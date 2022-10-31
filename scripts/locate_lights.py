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


def _sort_marks(img_path, n_marks):
    img = cv2.imread(img_path).astype(int)
    start_point = _locate_marks(img, n_marks=1, rgb='r')[0]
    points = _locate_marks(img, n_marks=n_marks-1, rgb='g')
    result = [start_point]
    remaining = set(points)
    while remaining:
        prev_x, prev_y = result[-1]
        neighbor = None
        min_dist = 2
        for x, y in remaining:
            dist = (x - prev_x) ** 2 + (y - prev_y) ** 2
            if min_dist > dist:
                min_dist = dist
                neighbor = x, y
        result.append(neighbor)
        remaining -= {neighbor}
    return result


class LEDLocator:
    def __init__(self, theme):
        self.theme = theme
        self.source_image = os.path.join(theme, f'{theme}.jpg')
        self.grayscale_images = []
        self.coordinates = None

    def convert_to_grayscale(self, n_copies=1):
        img = cv2.imread(self.source_image, 0)
        for i in range(n_copies):
            dst_name = f'{self.theme}_gs{i}.jpg'
            dst_path = os.path.join(self.theme, dst_name)
            if os.path.exists(dst_path):
                print('file {} already exists, will not overwrite'.format(dst_path))
            else:
                cv2.imwrite(dst_path, img)
            self.grayscale_images.append(dst_path)

    def locate_lights(self, num_marks, reversed=False):
        result = []
        for i, fn in enumerate(self.grayscale_images):
            curve = _sort_marks(fn, n_marks=num_marks[i])
            result.extend(curve)
        if reversed:
            result = result[::-1]
        result = np.array(result)
        result -= result.mean(axis=0)
        max_r = np.sqrt(result[:, 0] ** 2 + result[:, 1] ** 2).max()
        # scale and shrink to fit in unit circle
        result /= max_r
        self.coordinates = result

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

    def generate_code(self):
        dst = os.path.join('..', 'examples', f'{self.theme}', f'{self.theme}.ino')
        if os.path.exists(dst):
            print(f'{dst} already exists, will not overwrite')
            return
        x_coords = [int(round(x * 10000)) for x in self.coordinates[:, 0]]
        y_coords = [int(round(y * 10000)) for y in self.coordinates[:, 1]]
        with open('template.ino', 'r') as fp:
            content = fp.read()
        s = str(x_coords).replace('[', '{').replace(']', '}')
        x_coords_line = f'int16_t x_coords[{len(x_coords)}] = {s};'
        content = content.replace('{{x_coords}}', x_coords_line)
        s = str(y_coords).replace('[', '{').replace(']', '}')
        y_coords_line = f'int16_t y_coords[{len(x_coords)}] = {s};'
        content = content.replace('{{y_coords}}', y_coords_line)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        with open(dst, 'w') as fp:
            fp.write(content)


def mark_lights(locator, theme_name):
    image_name = os.path.join(theme_name, f'{theme_name}.jpg')
    n_segments = int(input('How many piecewise smooth curves are there?\n'))
    locator.convert_to_grayscale(image_name, n_copies=n_segments)
    print('Please mark each gray scale image, then update the configs map.')


def parameterize_lights(locator, num_marks, reversed):
    locator.locate_lights(num_marks, reversed=reversed)
    locator.generate_code()
    locator.render()


if __name__ == '__main__':
    """
    How to generate code for the heart light:
    Step 0. shape your LED lights (WS2812B, WS2813, etc.), power on and take a photo
    Step 1. save the photo to ./heart/heart.jpg under this directory
    Step 2. determine how many piecewise smooth components do you need to parameterize the curve,
            in this case 2: left half and right half.
    Step 3. run the script and follow the prompt to manually mark the LEDs on the gray scale images
         3.1. for each piecewise smooth curve, mark the first point RED and the rest GREEN
         3.2. count how many marks (RED + GREEN) on each curve (left: 31, right: 29)
         3.3. after marking all the points, update the configs map below
    Step 4. code is generated to ../examples/heart/heart.ino
    """
    theme_name = 'heart'
    configs = {'jack_o_lantern': {'num_marks': (34, 57, 26, 3), 'reversed': False},
               'heart': {'num_marks': (31, 29), 'reversed': False},
               'figure_three': {'num_marks': (25, 35), 'reversed': True}}
    locator = LEDLocator(theme=theme_name)
    if theme_name not in configs:
        mark_lights(locator, theme_name)
    else:
        n_segments = len(configs[theme_name]['num_marks'])
        locator.convert_to_grayscale(n_copies=n_segments)
    if theme_name in configs:
        parameterize_lights(locator, **configs[theme_name])
