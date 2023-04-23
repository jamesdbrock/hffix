# https://github.com/jonascarpay/template-haskell/blob/master/flake.nix
{
  description = "spec-parse-fields";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs = inputs:
    let
      overlay = final: prev: {
        haskell = prev.haskell // {
          packageOverrides = hfinal: hprev:
            prev.haskell.packageOverrides hfinal hprev // {
              spec-parse-fields = hfinal.callCabal2nix "spec-parse-fields" ./. { };
              # https://github.com/ddssff/listlike/issues/23
              ListLike = prev.haskell.lib.dontCheck hprev.ListLike;
            };
        };
      };
      perSystem = system:
        let
          pkgs = import inputs.nixpkgs { inherit system; overlays = [ overlay ]; };
          # hspkgs = pkgs.haskellPackages;
          hspkgs = pkgs.haskell.packages.ghc944;
        in
        {
          devShell = hspkgs.shellFor {
            withHoogle = true;
            packages = p: [ p.spec-parse-fields ];
            # packages = p: with p; [
            #   text
            #   text-format
            #   containers
            #   xml-types
            #   xml-conduit
            #   data-default-class
            #   attoparsec
            #   mtl
            # ];
            buildInputs = [
              hspkgs.cabal-install
              hspkgs.haskell-language-server
              # hspkgs.hlint
              # hspkgs.ormolu
              # pkgs.bashInteractive
              # pkgs.boost
              # pkgs.oxygen
              # # pkgs.stack
              # pkgs.gmp
              # pkgs.zlib
            ];
          };
        };
    in
    { inherit overlay; } // inputs.flake-utils.lib.eachDefaultSystem perSystem;
}
