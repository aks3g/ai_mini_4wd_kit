## How to build
### はじめに
このディレクトリは、micro Pythonのportsディレクトリ以下に置かれることを期待しています。
リポジトリ全体をcloneしている場合には、micro Pythonのコードをすべて/src/micro_pythonに展開すると配置がうまく行くはずです。

### 準備物
makeを通す為には、追加でMicrochip（Atmel）が提供している、cmsisとDFPのファイルを用意する必要があります。
WindowsでAtmel Studioをインストールし、SAMD51系のプロジェクトを作ると
C:\Program Files (x86)\Atmel\Studio\7.0\packs　以下に生成されます。

（間に合ってないですが、リセットハンドラ等を使いたい為だけなので、どこかのタイミングで依存は切ります）

### make
Makefileそのものの書き換えは不要ですが、2つの環境変数
1. CROSS_COMPILE : ARM-GCCの場所を示す(/path/to/arm-gcc/bin/arm-none-eabi- を指定)
2. PACKS_PATH : 上記、Microchip謹製のファイルが置いてある場所(/path/to/packs/)

を指定する必要があります。
2つの環境変数が正しく設定できていれば、make一発でelfファイルを生成することが出来ます。

