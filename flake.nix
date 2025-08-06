{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
  };
  outputs = { self, nixpkgs, ... }@inputs:
    let
      pkgs = import nixpkgs { system = "x86_64-linux"; };
    in
      {
        devShells.x86_64-linux.default = pkgs.mkShell {
          GRUB2_CROSS_PC="${pkgs.pkgsCross.i686-embedded.buildPackages.grub2}/lib/grub/i386-pc/";
          nativeBuildInputs =
            with pkgs; [
              pkgsCross.i686-embedded.buildPackages.gcc
              pkgsCross.i686-embedded.buildPackages.grub2
              libisoburn
              nasm
            ];        
        };
      };
}
