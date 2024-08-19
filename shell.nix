with import <nixpkgs> { };

mkShell {
  buildInputs = [ gperf gtest.dev pkg-config gcovr ];

  NIX_ENFORCE_PURITY = true;

  shellHook = ''
  '';
}
