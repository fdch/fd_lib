#!/bin/bash
#
# release the library to deken
#
#
VERSION="v$(grep version README.txt | cut -f 2 -d: | sed 's/ //')"
TAGS=$(git tag -l)
FORCE=0
if [[ "$1" == "-f" ]]; then
	FORCE=1
fi
for tag in ${TAGS}; do
	if [[ $VERSION == $tag ]]; then
		echo "Version $tag already exists. Increment version."
		if [[ $FORCE == 1 ]]; then
			echo "Attempting to force release."
			git push --delete origin $VERSION
			git tag -d $VERSION
		else
			exit 1
		fi
	fi
done
echo "Releasing tag $VERSION"
git tag $VERSION -m "Release $VERSION"
git push origin tag $VERSION
