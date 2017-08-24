COUNT_MATRIX_DOWNSIZER=./count-matrix-downsizer

TEST_DATA=/mnt/nfsdata/projects/BIO027/final_tables/BIO027.mocat_abundance.txt
TEST_DIR=tests/input/2.help_message

TARGET_READ_COUNT=5000000
NUM_REPETITIONS=30
DOWNSIZED_MATRIX=${TEST_DIR}/BIO027.downsized.5M.txt
STATS=${TEST_DIR}/BIO027.richness_genes.5M.txt

${COUNT_MATRIX_DOWNSIZER} -h

