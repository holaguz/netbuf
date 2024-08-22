with import <nixpkgs> { };

let
  pkgs_cross = import <nixpkgs> {
    localSystem = "x86_64-linux";
    crossSystem = {
      config = "armv7l-unknown-linux-gnueabihf"; # see pkgs.lib.systems.examples
    };
  };

  shell = mkShell {

    buildInputs = [
      gperf
      gtest.dev
      pkg-config
      gcovr
      qemu
    ];

  };

  arm-shell = pkgs_cross.mkShell {

    nativeBuildInputs = [
      qemu
      pkg-config
      gcovr
      (writeShellScriptBin "run" ''
        make build/main
        qemu-arm build/main
      '')
    ];

  };

in
{
  inherit shell;
  inherit arm-shell;
}
