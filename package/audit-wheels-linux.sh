# Bundle external shared libraries into the wheels
WHEEL_DIR="wheels/"
OUT_DIR="wheelhouse"
mkdir -p ${OUT_DIR}
for whl in ${WHEEL_DIR}/*.whl; do
   auditwheel repair "$whl" -w ${OUT_DIR} --plat linux_x86_64
done
ls -ltrh ${OUT_DIR}
