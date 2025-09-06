#!/bin/bash 
SAMPLE_FILES_DIR="sample_files"
CURL_TEST_FILE_1="15MBFILE.txt"
CURL_TEST_FILE_2="46MBVIDEO.mp4"
CURL_TEST_FILE_3="20MBPDF.pdf"
ENCRYPTED_FILE_5MB="5MB_encrypted.bin"
ENCRYPTED_FILE_10MB="10MB_encrypted.bin"
ENCRYPTED_FILE_20MB="20MB_encrypted.bin"
TEST_PASSWORD="12345"

if [[ -z "$(which curl)" ]];then
  echo "Error: 'curl' is required to download dependencies."
  echo "Please install curl and try again."
  exit 1
    exit 1
fi

if [[ -z "$(which openssl)" ]];then
  echo "Error: 'openssl' is required to download dependencies."
  echo "Please install openssl and try again."
  exit 1
    exit 1
fi

# Function to check if all required sample files exist.
check_files_exist() {
  if [[ -f "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_1}" ]] && \
     [[ -f "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_2}" ]] && \
     [[ -f "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_3}" ]]; then
    return 0 # All files found
  else
    return 1 # One or more files are missing
  fi
}

check_enc_files_exist(){
if [[ -f "${SAMPLE_FILES_DIR}/${ENCRYPTED_FILE_5MB}" ]] && \
     [[ -f "${SAMPLE_FILES_DIR}/${ENCRYPTED_FILE_10MB}" ]] && \
     [[ -f "${SAMPLE_FILES_DIR}/${ENCRYPTED_FILE_20MB}" ]]; then
    return 0 # All files found
  else
    return 1 # One or more files are missing
  fi
}

if check_files_exist; then
  echo "Sample files already exist, skipping download."
else
  echo "Sample files not found. Checking internet connection..."
  ping -c 1 -W 2 1.1.1.1 > /dev/null 2>&1
  if [ $? -ne 0 ]; then
    echo "No internet connection detected. Cannot download files."
    exit 1
  fi

  echo "Internet connection is available. Downloading files..."
  mkdir -p "${SAMPLE_FILES_DIR}"

  echo "Downloading ${CURL_TEST_FILE_1}..."
  curl -s https://link.testfile.org/15MB -o "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_1}"
  echo "Downloading ${CURL_TEST_FILE_2}..."
  curl -s https://link.testfile.org/ihDc9s -o "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_2}"
  echo "Downloading ${CURL_TEST_FILE_3}..."
  curl -s https://link.testfile.org/PDF20MB -o "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_3}"

  if ! check_files_exist; then
    echo "Error: One or more files failed to download."
    exit 1
  else 
   echo  "Pass: All files downloaded"
  fi
fi


if check_enc_files_exist; then
  echo "Encrypted sample files already exist, skipping generation."
else
  echo "Encrypted sample files not found. Generating new ones..."
  [[ ! -d "${SAMPLE_FILES_DIR}" ]] && mkdir -p "${SAMPLE_FILES_DIR}"

  echo "Generating 5MB encrypted file..."
  dd if=/dev/urandom of="${SAMPLE_FILES_DIR}/5MB_plain.bin" bs=1M count=5 > /dev/null 2>&1
  openssl enc -aes-256-cbc -in "${SAMPLE_FILES_DIR}/5MB_plain.bin" -out "${SAMPLE_FILES_DIR}/${ENCRYPTED_FILE_5MB}" -k "${TEST_PASSWORD}"
  rm "${SAMPLE_FILES_DIR}/5MB_plain.bin" 

  echo "Generating 10MB encrypted file..."
  dd if=/dev/urandom of="${SAMPLE_FILES_DIR}/10MB_plain.bin" bs=1M count=10 > /dev/null 2>&1
  openssl enc -aes-256-cbc -in "${SAMPLE_FILES_DIR}/10MB_plain.bin" -out "${SAMPLE_FILES_DIR}/${ENCRYPTED_FILE_10MB}" -k "${TEST_PASSWORD}"
  rm "${SAMPLE_FILES_DIR}/10MB_plain.bin"

  echo "Generating 20MB encrypted file..."
  dd if=/dev/urandom of="${SAMPLE_FILES_DIR}/20MB_plain.bin" bs=1M count=20 > /dev/null 2>&1
  openssl enc -aes-256-cbc -in "${SAMPLE_FILES_DIR}/20MB_plain.bin" -out "${SAMPLE_FILES_DIR}/${ENCRYPTED_FILE_20MB}" -k "${TEST_PASSWORD}"
  rm "${SAMPLE_FILES_DIR}/20MB_plain.bin"

  if ! check_enc_files_exist; then
    echo "Error: One or more encrypted files failed to be generated."
    exit 1
  fi
  echo "All encrypted files generated successfully."
fi

sleep 1
echo -e "\nRunning tests on sample files..."
sleep 0.5
for file in "${SAMPLE_FILES_DIR}"/*; do
  if [[ -f "$file" ]]; then
    echo "=========================================================================="
    echo "Testing: $file"
    ../build/detectenc "$file"
    echo
  fi
done
