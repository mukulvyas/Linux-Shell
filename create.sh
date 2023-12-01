#!/bin/bash

# Function to create directory with files of specified size and count
generate_files() {
  local file_size=$1
  local file_count=$2
  local directory_name=$3

  mkdir -p "${directory_name}" &&
  cd "${directory_name}" &&
  for index in $(seq 1 "${file_count}"); do
    dd if=/dev/zero of=file_${index}.dat bs="${file_size}" count=1 status=none &
  done
  wait
}

generate_recursive_structure() {
  local base_directory=$1
  local subdir_count=$2
  local file_size=$3

  generate_files "${file_size}" 1 "${base_directory}"

  for subdir_index in $(seq 1 "${subdir_count}"); do
    generate_files "${file_size}" 100 "${base_directory}/subdir_${subdir_index}" &
  done
  wait
}

while true; do
  read -p "Choose:
1. Generate 100 files, each 1GB in size
2. Generate 10,000 files, each 10MB in size
3. Generate 10MB files, 100 files directly, and recursively create subdirectories (total 10,000 files)
Enter The Choice : " user_choice

  case ${user_choice} in
    1)
      echo "Option 1..."
      time (generate_files "10MB" 100 "1st_direct")
      ;;
    2)
      echo "Option 2..."
      time (generate_files "5MB" 1000 "2nd_direct")
      ;;
    3)
      echo "Option 3..."
      time ( generate_files "1M" 10 "3rd_recursive" &&
        for subdir_index in $(seq 1 99); do
          generate_files "1M" 10 "subdir_${subdir_index}"
        done)
      ;;
    *)
      echo "Invalid choice. Exiting."
      exit 1
      ;;
  esac

  read -p "run another option? (y/n): " run_again
  if [[ "${run_again}" != "y" ]]; then
    echo "Exiting."
    break
  fi
done

echo "All done"

