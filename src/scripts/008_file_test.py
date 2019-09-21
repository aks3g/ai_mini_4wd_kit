# Sample Name: 008_file_test.py
# 概要: SDカード上のファイルへのアクセスを行います
#
# Copyright 2019 Kiyotaka Akasaka. All rights reserved.
#
import mini4wd
f = open("data.txt", 'w')
f.write("test text")
f.close()
