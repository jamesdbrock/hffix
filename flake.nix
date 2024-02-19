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
    in
    {
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
    });
}
