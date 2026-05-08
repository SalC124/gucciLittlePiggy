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
      in
      {
        formatter = pkgs.nixfmt;
        devShells.default = pkgs.mkShell.override { stdenv = pkgs.devkitNix.stdenvARM; } {
          buildInputs = with pkgs; [
            bear
          ];
          shellHook = ''
            export CTRULIB="$DEVKITPRO/libctru"
            export DEVKITARM="$DEVKITPRO/devkitARM"
            # instead of the standard nixpkgs arm-none-eabi-gcc
            export PATH="$DEVKITARM/bin:$PATH"
          '';
        };
      }
    );
}
