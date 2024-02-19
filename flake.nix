{
  description = "hffix";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-23.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, ... }@inputs:
    inputs.flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs { inherit system; };
      version = "1.4.0";
    in
    rec {
      devShells.default = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [ doxygen stack gmp zlib ];
        buildInputs = with pkgs; [ boost ];
        LC_ALL = "C.UTF-8";
        shellHook = ''
          echo ""
          echo '  hffix Development Environment'
          echo '  To run the test suite: make test'
          echo ""
        '';
      };
      packages = rec {
        hffix = pkgs.stdenv.mkDerivation {
          pname = "hffix";
          inherit version;
          src = ./include;
          meta = with pkgs.lib; {
            description = "Header-only C++ library for FIX (Financial Information eXchange) protocol";
            license = licenses.mit;
            platforms = platforms.all;
          };
          unpackPhase = ''
          '';
          buildPhase = ''
          '';
          installPhase = ''
            mkdir -p $out/include
            cp -r $src/* $out/include
          '';
        };
        fixprint = pkgs.stdenv.mkDerivation {
          pname = "fixprint";
          inherit version;
          srcs = [./util/src];
          nativeBuildInputs = [ hffix ];
          sourceRoot = ".";
          meta = with pkgs.lib; {
            description = "Print FIX messages from stdin to stdout in human-readable format";
            license = licenses.mit;
            platforms = platforms.all;
          };
          buildPhase = ''
            $CXX -o fixprint src/fixprint.cpp
          '';
          installPhase = ''
            mkdir -p $out/bin
            mv fixprint $out/bin/fixprint
          '';
        };
      };
      apps = {
        fixprint = {
          type = "app";
          program = "${packages.fixprint}/bin/fixprint";
        };
      };
    });
}
