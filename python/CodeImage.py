import pytesseract

from PIL import Image

image = Image.open(r'D:\workspace\Python\test.png')

vcode = pytesseract.image_to_string(image)

print (vcode)
