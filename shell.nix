# In the nix-shell environment, this derivation should provide everything
# needed for all of the Makefile targets

{ pkgs ? import (builtins.fetchGit {
  # https://github.com/NixOS/nixpkgs/releases/tag/21.11
  url = "https://github.com/nixos/nixpkgs/";
  ref = "refs/tags/21.11";
  rev = "a7ecde854aee5c4c7cd6177f54a99d2c1ff28a31";
  }) {}
}:

pkgs.stdenv.mkDerivation {
# clangStdenv.mkDerivation { -- TODO clang
  name = "hffix";
  src = ./.;

  buildInputs = with pkgs; [ boost doxygen stack gmp zlib ];
#   buildInputs = [ boost doxygen stack gmp zlib clang]; -- TODO clang
  shellHook = ''
    export LANG=C.UTF-8
    echo ""
    echo '  hffix Development Environment'
    echo '  To run the test suite: make test'
    echo ""
  '';
}
