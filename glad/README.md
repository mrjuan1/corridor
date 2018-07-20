GLAD
===
I use glad, it's good. See https://github.com/Dav1dde/glad

Setup glad:
===
```
git clone --depth=1 https://github.com/Dav1dde/glad
cd glad
python setup.py build
sudo python setup.py install # Or leave out sudo if you're using Windows
```

Use glad:
===
Generated with:
```
glad --profile core --out-path . --api gl= --generator c --extensions GL_EXT_texture_filter_anisotropic --spec gl --omit-khrplatform --local-files
```
