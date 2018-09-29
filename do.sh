rm /home/melalonso/cgi-bin/*
make
for f in *; do
  if [ -f "$f" ] && [ -x "$f" ] && [ "$f" != 'do.sh' ] && [ "$f" != 'Makefile' ]; then
    mv "$f" /home/melalonso/cgi-bin
  fi
done
rm *.o