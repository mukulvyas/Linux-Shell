#!/bin/bash

# Function to create directory with files of specified size and count
create_directory() {
  local size=$1
  local count=$2
  local dirname=$3

  mkdir -p "${dirname}" &&
  cd "${dirname}" &&
  for i in $(seq 1 "${count}"); do
    dd if=/dev/zero of=file_${i}.dat bs="${size}" count=1 status=none
  done
}

execute_case() {
  local size=$1
  local count=$2
  local dirname=$3

  create_directory "${size}" "${count}" "${dirname}"
}

while true; do
  read -p "Choose a case to execute:
1. 100 files of 1GB each
2. 10,000 files of 10MB each
3. 10MB files, 100 files directly, and recursively create subdirectories (total 10,000 files)
Enter your choice (1/2/3): " choice

  case ${choice} in
    1)
      echo "Executing Case 1..."
      time (execute_case "1M" 10 "dir_1gb") &
      ;;
    2)
      echo "Executing Case 2..."
      time (execute_case "10M" 10 "dir_10mb") &
      ;;
    3)
	  echo "Executing Case 3..."
	  time (
	    execute_case "10M" 10 "dir_recursive" &  # Create initial directory with 10 files
	    wait  # Wait for background process to finish
	    for i in $(seq 1 10); do
	      execute_case "10M" 10 "subdir_${i}" &  # Create 10 subdirectories, each with 10 files
	    done
	    wait  # Wait for all background processes to finish
	  )
	  ;;

    *)
      echo "Invalid choice. Exiting."
      exit 1
      ;;
  esac

  read -p "Do you want to run another case? (y/n): " run_again
  if [[ "${run_again}" != "y" ]]; then
    echo "Exiting."
    break
  fi
done

echo "All directories and files created successfully."
