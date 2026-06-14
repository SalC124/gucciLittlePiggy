{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    devkitNix.url = "github:bandithedoge/devkitNix";
  };

  outputs =
    {
      nixpkgs,
      flake-utils,
      devkitNix,
      ...
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [ devkitNix.overlays.default ];
        };

        gen-clangd = pkgs.writeShellScriptBin "gen-clangd" ''
            # 1. Ask the compiler for its version (e.g., 15.2.0)
            GCC_VERSION=$($DEVKITARM/bin/arm-none-eabi-gcc -dumpversion)

            # 2. Construct the base C++ path
            CPP_BASE="$DEVKITARM/arm-none-eabi/include/c++/$GCC_VERSION"

            # 3. Fallback: If for some reason dumpversion fails, find the first folder in c++ directory
            if [ ! -d "$CPP_BASE" ]; then
                CPP_BASE=$(find "$DEVKITARM/arm-none-eabi/include/c++" -mindepth 1 -maxdepth 1 -type d | head -n 1)
            fi

            cat <<EOF > .clangd
            CompileFlags:
            Add:
            - "-Wno-unknown-argument"
            - "--target=arm-none-eabi"
            - "-D__3DS__"
            - "-D__arm__"
            - "-DARM11"
            - "-isystem"
            - "$CPP_BASE"
            - "-isystem"
            - "$CPP_BASE/arm-none-eabi"
            - "-isystem"
            - "$DEVKITARM/arm-none-eabi/include"
            - "-isystem"
            - "$CTRULIB/include"

            Remove:
            - "-mword-relocations"
            - "-march=*"
            - "-mtune=*"

            Index:
            Background: Build
            EOF
            echo "Generated .clangd for arm-none-eabi-g++ version $GCC_VERSION"
        '';

      in
      {
        devShells.default = pkgs.mkShell.override { stdenv = pkgs.devkitNix.stdenvARM; } {
          buildInputs = with pkgs; [
            bear
            gen-clangd
          ];

          shellHook = ''
            export CTRULIB="$DEVKITPRO/libctru"
            export DEVKITARM="$DEVKITPRO/devkitARM"
            export PATH="$DEVKITARM/bin:$PATH"
          '';
        };
      }
    );
}
