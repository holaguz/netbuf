with import <nixpkgs> { };

mkShell {
  buildInputs = [ gperf gtest.dev ];
  nativeBuildInputs = [ pkg-config ];

  NIX_ENFORCE_PURITY = true;

  shellHook = ''
  '';
}
