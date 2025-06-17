# compute_phash.py
import sys
import cv2
import numpy as np

def compute_phash(img):
    img = cv2.resize(img, (32, 32), interpolation=cv2.INTER_AREA)
    img = np.float32(img)
    dct = cv2.dct(img)
    dct_low = dct[:8, :8]
    vals = dct_low.flatten()[1:]  # skip DC
    median = np.median(vals)
    bits = vals > median
    hash = 0
    for i, bit in enumerate(bits):
        if bit:
            hash |= 1 << i
    return format(hash, '064b')  # <-- 回傳 64 位元的二進位字串

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("error: usage compute_phash.py <image_path>")
        sys.exit(1)
    path = sys.argv[1]
    img = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        print(f"error: cannot read image {path}")
        sys.exit(1)
    print(compute_phash(img))
