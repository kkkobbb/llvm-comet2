FROM alpine

# 必要なパッケージのインストール
RUN apk update && apk add cmake gcc g++ python zlib-dev make git

# ユーザ関係設定
ENV USER test
ENV HOME /home/${USER}

ENV BUILD_DIR _build_comet2_llc

# ユーザ追加 パスワード未設定
RUN adduser -D ${USER}

# 以降実行するユーザ
USER ${USER}
WORKDIR ${HOME}

# イメージ生成時の準備
RUN git clone https://github.com/llvm/llvm-project.git
RUN git clone https://github.com/kkkobbb/llvm-comet2.git
RUN cd llvm-project/ && mkdir ${BUILD_DIR} && cd llvm/lib/Target && ln -s ${HOME}/llvm-comet2/Comet2

# コンテナ起動時に実行
# メモリ 1G + swap 4Gで成功
ENV TAG llvmorg-9.0.0
CMD git -C ./llvm-comet2 pull && cd ${HOME}/llvm-project && git checkout ${TAG} && git apply ${HOME}/llvm-comet2/patches/comet2.patch && cd ${BUILD_DIR} && cmake ../llvm -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=Comet2 -DLLVM_TARGETS_TO_BUILD=Comet2 && make llc && ./bin/llc --mtriple=comet2 -debug ${HOME}/llvm-comet2/tests/add_reg_const.ll

