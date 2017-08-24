COUNT_MATRIX_DOWNSIZER=./count-matrix-downsizer

TEST_DATA=/mnt/nfsdata/projects/BIO027/final_tables/BIO027.mocat_abundance.txt
TEST_DIR=tests/input/9.non_writable_downsized_matrix

TARGET_READ_COUNT=5000000
NUM_REPETITIONS=30
DOWNSIZED_MATRIX=/BIO027.downsized.5M.txt
STATS=${TEST_DIR}/BIO027.richness_genes.5M.txt

${COUNT_MATRIX_DOWNSIZER} --count-matrix ${TEST_DATA} --target-read-count ${TARGET_READ_COUNT} --num-repetitions ${NUM_REPETITIONS} --downsized-matrix ${DOWNSIZED_MATRIX} --stats ${STATS}
