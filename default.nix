{ pkgs ? (import <nixpkgs> {}) }:

# Use this config for `nix-shell --pure` only.
# WARNING: for some reason QTcpServer::listen will always fail inside of
#          the nix-shell, I have yet to figure out why.
# Follow the README for build instructions.

pkgs.stdenv.mkDerivation {
  name = "qt-media-u-env";
  buildInputs = [
    pkgs.stdenv pkgs.qt5Full pkgs.mesa pkgs.gnumake # actually needed for build
    pkgs.x11 pkgs.qtcreator pkgs.gdb # debug only
    pkgs.cmake # libraries build only
  ];
}
