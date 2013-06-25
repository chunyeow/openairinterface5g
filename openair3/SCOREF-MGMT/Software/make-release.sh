
# Verify parameters
if [ $# -ne 1 ]
then
  echo "Usage: `basename $0` <version>"
  exit
fi

VERSION=$1
BINARY_NAME=SCOREF-MGMT
RELEASE_NAME="$BINARY_NAME-$VERSION"

echo "Building the version $VERSION"
make clean &> /dev/null
make BUILD_STATIC=1 -j &> /dev/null
strip bin/$BINARY_NAME

echo "Preparing the release..."
mkdir $RELEASE_NAME
echo "Copying binary..."
cp bin/$BINARY_NAME $RELEASE_NAME

echo "Copying configuration files..."
cp bin/MGMT.conf bin/configuration -r $RELEASE_NAME
rm $RELEASE_NAME/configuration/.svn -rfv &> /dev/null

echo "Copying documentation..."
cp README README.RUNNING $RELEASE_NAME

echo "Preparing the compressed archive..."
tar cjvf $RELEASE_NAME.tar.bz2 $RELEASE_NAME &> /dev/null

echo "Removing temporary directory..."
rm $RELEASE_NAME -rf &> /dev/null

echo "Release is ready"
exit

