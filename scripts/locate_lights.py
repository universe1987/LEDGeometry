import cv2
import numpy as np
from sklearn.cluster import KMeans


def convert_to_grayscale(src_path, dst_path):
    img = cv2.imread(src_path, 0)
    cv2.imwrite(dst_path, img)


def extract_marks(src_path, dst_path):
    """
    Assuming the marks are green and the background is gray, so the green channel stands out.
    """
    img = cv2.imread(src_path).astype(int)
    img_diff = img[:, :, 1] - img[:, :, 0]
    img_diff[img_diff > 32] = 255
    cv2.imwrite(dst_path, img_diff)


def locate_centers(src_path, n_lights):
    """
    Identify the center of the white dots from a black and white image using k-means.
    """
    img = cv2.imread(src_path, 0)
    y, x = np.where(img == 255)
    y = y.astype(float) / img.shape[0]
    x = x.astype(float) / img.shape[1]
    km = KMeans(n_clusters=n_lights).fit(np.vstack([x, y]).T)
    centers = km.cluster_centers_
    centers -= centers.mean(axis=0)
    max_r = np.sqrt(centers[:, 0] ** 2 + centers[:, 1] ** 2).max()
    # scale and shrink to fit in unit circle
    centers /= max_r
    return centers


def connect_the_dots(centers, start, is_left_half):
    """
    Map the index to coordinates, this assumes that the curve is symmetric to y-axis, and for each half of the curve,
    two dots are closest if and only if they are adjacent.
    """
    result = [start]
    for i in range(len(centers) // 2 - 1):
        cx, cy = result[-1]
        neighbor = None
        min_dist = 1
        chosen = set(result)
        for x, y in centers:
            if is_left_half and x > 0.02:
                continue
            if not is_left_half and x < -0.02:
                continue
            if (x, y) in chosen:
                continue
            dist = (x - cx) ** 2 + (y - cy) ** 2
            if min_dist > dist:
                min_dist = dist
                neighbor = x, y
        result.append(neighbor)
    return result


def sort_centers(centers, render=False):
    left_start, right_start = None, None
    for x, y in centers:
        if -0.03 < x < 0 and y < -0.3:
            print(x, y)
            left_start = x, y
        if 0 < x < 0.07 and y < -0.3:
            print(x, y)
            right_start = x, y
    left_half = connect_the_dots(centers, left_start, True)
    right_half = connect_the_dots(centers, right_start, False)
    result = left_half + right_half[::-1]
    x_coords = [a[0] for a in result]
    y_coords = [-a[1] for a in result]

    if render:
        import matplotlib.pyplot as plt
        plt.plot(x_coords, y_coords)
        plt.scatter([left_start[0], right_start[0]], [-left_start[1], -right_start[1]])
        plt.show()
    return x_coords, y_coords


def parametrize_leds():
    centers = list(list(x) for x in locate_centers('imgs/marks.jpg', 60))
    x_coords, y_coords = sort_centers(centers, True)
    x_coords = [int(round(x * 10000)) for x in x_coords]
    y_coords = [int(round(y * 10000)) for y in y_coords]
    s = str(x_coords).replace('[', '{').replace(']', '}')
    print('int16_t x_coords[60] = {};'.format(s))
    s = str(y_coords).replace('[', '{').replace(']', '}')
    print('int16_t y_coords[60] = {};'.format(s))


if __name__ == '__main__':
    # Step 0. take a photo of your light when the lights are on and save it to 'imgs/blue_heart.jpg'
    # Step 1. uncomment and run the following line to convert the photo to grayscale
    # convert_to_grayscale('imgs/blue_heart.jpg', 'imgs/gray.jpg')
    # Step 2. after manually mark the LEDs with green dots, for example, using Windows' paint, uncomment and run
    # the following line to extract your marks into a black and white image
    # extract_marks('imgs/gray.jpg', 'imgs/marks.jpg')
    # Step 3. run the following command and paste the output to the Arduino IDE.
    parametrize_leds()
