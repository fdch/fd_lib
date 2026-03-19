#!/bin/bash
#
# release the library to deken
#
#
VERSION="v$(grep version README.txt | cut -f 2 -d: | sed 's/ //')"
TAGS=$(git tag -l)
for tag in ${TAGS}; do
	if [[ $VERSION == $tag ]]; then
		echo "Version $tag already exists. Increment version."
		exit 1
	fi
done
echo "Releasing tag $VERSION"
git tag $VERSION -m "Release $VERSION"
git push origin tag $VERSION
