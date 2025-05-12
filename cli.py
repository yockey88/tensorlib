import os
import subprocess
import sys
import argparse

def recursive_compile_flatbuffers(path, output_dir="network/fb_specs"):
  """
  Recursively compile all .fbs files in the given directory and its subdirectories.
  """
  for root, dirs, files in os.walk(path):
    for file in files:
      if file.endswith(".fbs"):
        fbs_file = os.path.join(root, file)
        subprocess.run(["extern/flatbuffers/flatc", "-o", output_dir, "--cpp", fbs_file], check=True)
    
    for dir in dirs:
      dir_path = os.path.join(root, dir)
      recursive_compile_flatbuffers(dir_path, output_dir)

if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="A simple CLI for a Python project.")
  # -rs <sandbox-name> OR --run-sandbox <sandbox-name>
  parser.add_argument("--build", "-b", action="store_true", help="Build the project.")
  parser.add_argument("--run", "-r", action="store_true", help="Run the main driver.")
  parser.add_argument("--run-sandbox", "-rs", type=str, help="Run a sandbox with the given name.")
  parser.add_argument("--flatc", "-fbc", type=str, help="Compile a flatbuffer specification.")
  parser.add_argument("--build-all-flatbuffer-specs", "-baf", action="store_true", help="Build all flatbuffer specs.")

  args = parser.parse_args()
  try:
    if args.build:
      print("Building the project...")
      # build fbs first
      recursive_compile_flatbuffers("network/fb_specs")
      subprocess.run(["ninja"], check=True, cwd="build")

      # check if run is set for run then build
      if args.run:
        subprocess.run(["build/network/network_driver.exe"], check=True)

    elif args.run:
      subprocess.run(["build/network/network_driver.exe"], check=True)

    elif args.run_sandbox:
      subprocess.run([f"build/sandboxes/{args.run_sandbox}.exe"], check=True)

    elif args.flatc:
      subprocess.run(["extern/flatbuffers/flatc", "--cpp", args.flatc], check=True)

    elif args.build_all_flatbuffer_specs:
      recursive_compile_flatbuffers("network/fb_specs")

    else:
      parser.print_help()
      
  except subprocess.CalledProcessError as e:
    print(f"Error: {e}")
    sys.exit(1)
