#!/bin/bash
# View a blog in your preferred browser: First argument is the blog entry,
# second argument is the browser application

blog=$1
browser=$2

temp=$1-temp.html

if [[ -f "$1" ]]
then
	touch "$temp"
	echo "<html><style>p,img{margin-left:2vw; margin-right:2vw;}gl{color:orange}
		img{display: block; width: 70%;}</style><head><title>$blog</title></head><body>
		<h3>THIS IS A TEMPORARY PREVIEW OF THE BLOG.
		THE REAL BLOG IS HOSTED ON <a href=\"https://dangers.works\">dangers.works</a></h3><br>" >> "$temp"
	cat "$1" >> "$temp"
	echo "</body></html>" >> "$temp"
	$2 "$temp"
	rm "$temp"
else
	echo This blog entry does not exist.
fi
