import argparse
import cv2
import numpy as np
import os
from sklearn.cluster import KMeans


def _locate_marks(img, n_marks, rgb='r'):
    """
    Locate color marks on a gray scale image, all marks must be the same color from one of the 3 primary colors.
    :param img: 3-channel image
    :param n_marks: the number of marks in the color specified below
    :param rgb: one of 'r', 'g', 'b'
    :return: an unordered list of coordinates, coordinates are normalized to [0, 1)
    """
    channel = {'r': 2, 'g': 1, 'b': 0}[rgb]
    img_diff = img[:, :, channel] - img.mean(axis=2)
    y, x = np.where(img_diff > 32)
    scale = max(img_diff.shape)
    y = y.astype(float) / scale
    x = x.astype(float) / scale
    km = KMeans(n_clusters=n_marks).fit(np.vstack([x, y]).T)
    return [tuple(p) for p in km.cluster_centers_]


def _parameterize_smooth_curve(img_path, n_points):
    img = cv2.imread(img_path).astype(int)
    start_point = _locate_marks(img, n_marks=1, rgb='r')[0]
    print(start_point)
    points = _locate_marks(img, n_marks=n_points-1, rgb='g')
    result = [start_point]
    for i in range(len(points)):
        cx, cy = result[-1]
        neighbor = None
        min_dist = 1
        chosen = set(result)
        for x, y in points:
            if (x, y) in chosen:
                continue
            dist = (x - cx) ** 2 + (y - cy) ** 2
            if min_dist > dist:
                min_dist = dist
                neighbor = x, y
        result.append(neighbor)
    return result


class LEDLocator:
    def __init__(self):
        self.source_image = None
        self.grayscale_images = []
        self.coordinates = None

    def convert_to_grayscale(self, src_path, n_copies=1):
        src_file_name, src_extension = os.path.basename(src_path).rsplit('.', 1)
        dst_dir = os.path.dirname(src_path)
        img = cv2.imread(src_path, 0)
        for i in range(n_copies):
            dst_name = f'{src_file_name}_gs{i}.{src_extension}'
            dst_path = os.path.join(dst_dir, dst_name)
            if os.path.exists(dst_path):
                print('file {} already exists, will not overwrite'.format(dst_path))
            else:
                cv2.imwrite(dst_path, img)
            self.grayscale_images.append(dst_path)

    def parameterize(self, points_per_curve, reversed=False):
        result = []
        for i, fn in enumerate(self.grayscale_images):
            curve = _parameterize_smooth_curve(fn, n_points=points_per_curve[i])
            result.extend(curve)
        if reversed:
            result = result[::-1]
        result = np.array(result)
        result -= result.mean(axis=0)
        max_r = np.sqrt(result[:, 0] ** 2 + result[:, 1] ** 2).max()
        # scale and shrink to fit in unit circle
        result /= max_r
        self.coordinates = result
        self._generate_code()

    def render(self):
        x_coords = [255 + int(255*x) for x in self.coordinates[:, 0]]
        y_coords = [255 + int(255*y) for y in self.coordinates[:, 1]]
        img = np.zeros((512, 512, 3), np.uint8)
        prev_x, prev_y = None, None
        for x, y in zip(x_coords, y_coords):
            if prev_x is not None:
                cv2.line(img, (prev_x, prev_y), (x, y), (0, 255, 0), 3)
            prev_x, prev_y = x, y
        for x, y in zip(x_coords, y_coords):
            cv2.circle(img, (x, y), 5, (0, 0, 255), -1)
        cv2.imshow('image', img)
        cv2.waitKey(0)
        cv2.destroyAllWindows()

    def _generate_code(self):
        x_coords = [int(round(x * 10000)) for x in self.coordinates[:, 0]]
        y_coords = [int(round(y * 10000)) for y in self.coordinates[:, 1]]
        s = str(x_coords).replace('[', '{').replace(']', '}')
        print('int16_t x_coords[{}] = {};'.format(len(x_coords), s))
        s = str(y_coords).replace('[', '{').replace(']', '}')
        print('int16_t y_coords[{}] = {};'.format(len(x_coords), s))


def process_image(image_name, n_segments, reversed):
    locator = LEDLocator()
    locator.convert_to_grayscale(image_name, n_copies=n_segments)
    points_per_curve = input('Please mark each gray scale image, then enter the number of dots separated by comma:\n')
    points_per_curve = [int(x.strip()) for x in points_per_curve.split(',')]
    locator.parameterize(points_per_curve, reversed=reversed)
    locator.render()


if __name__ == '__main__':
    # Step 0. take a photo of your light when the lights are on and save it.
    # Step 1. uncomment and run the following line to convert the photo to grayscale
    # convert_to_grayscale('imgs/blue_heart.jpg', 'imgs/gray.jpg')
    # Step 2. after manually mark the LEDs with green dots, for example, using Windows' paint, uncomment and run
    # the following line to extract your marks into a black and white image
    # extract_marks('imgs/gray.jpg', 'imgs/marks.jpg')
    # Step 3. run the following command and paste the output to the Arduino IDE.
    process_image(image_name='halloween.jpg', n_segments=4, reversed=False)
