#!/usr/bin/sh

mkdir -p resized
mkdir -p resized_floped
mkdir -p appended

for FILE in $(ls *.png)
do
	echo "resizing file $FILE"
	convert $FILE -resize 288x288 resized/$FILE
	convert $FILE -resize 288x288 -flop resized_floped/$FILE
done

# create one image for each direction
echo "creating image right"
convert resized/*.png +append -resize 2160x288\! appended/right.png
echo "creating image left"
convert resized_floped/*.png +append -resize 2160x288\! appended/left.png

# create own final image
echo "creating final image"
convert appended/right.png appended/left.png -append ../player_final.png

