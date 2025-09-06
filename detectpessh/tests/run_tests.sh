#!/bin/bash 
SAMPLE_FILES_DIR="sample_files"
CURL_TEST_FILE_1="putty.exe"
CURL_TEST_FILE_2="pscp.exe"
CURL_TEST_FILE_3="puttytel.exe"

if [[ -z "$(which curl)" ]];then
  echo "Error: 'curl' is required to download dependencies."
  echo "Please install curl and try again."
  exit 1
    exit 1
fi


# Function to check if all required sample files exist.
check_files_exist() {
  if [[ ! -f "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_1}" ]] || \
     [[ ! -f "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_2}" ]] || \
     [[ ! -f "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_3}" ]]; then
    return 1 # Return 1 if any file is missing
  fi
  return 0 # Return 0 if all files are found
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
  curl -L -s https://the.earth.li/~sgtatham/putty/latest/w64/putty.exe -o "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_1}"
  
  echo "Downloading ${CURL_TEST_FILE_2}..."
  curl -L -s https://the.earth.li/~sgtatham/putty/latest/w64/pscp.exe -o "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_2}"
  
  echo "Downloading ${CURL_TEST_FILE_3}..."
  curl -L -s https://the.earth.li/~sgtatham/putty/latest/w64/puttytel.exe -o "${SAMPLE_FILES_DIR}/${CURL_TEST_FILE_3}"

  if ! check_files_exist; then
    echo "Error: One or more files failed to download."
    exit 1
  else 
   echo  "Pass: All files downloaded"
  fi
fi



sleep 1
echo -e "\nRunning tests on sample files..."
sleep 0.5
for file in "${SAMPLE_FILES_DIR}"/*; do
  if [[ -f "$file" ]]; then
    echo "=========================================================================="
    echo "Testing: $file"
    ../build/detectpessh "$file"
    echo
  fi
done
