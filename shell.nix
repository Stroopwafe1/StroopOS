{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  GRUB2_CROSS_PC="${pkgs.pkgsCross.i686-embedded.buildPackages.grub2}/lib/grub/i386-pc/";
  nativeBuildInputs =
    with pkgs; [
      pkgsCross.i686-embedded.buildPackages.gcc
      pkgsCross.i686-embedded.buildPackages.grub2
      libisoburn
      nasm
    ];
  
}
