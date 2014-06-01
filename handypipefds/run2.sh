{ echo a; sleep 5; } | {
    exec 3<&0
{ echo b; sleep 1; echo c; } | {
    exec 4<&0
(./handypipefds 3 1 4 1 ; echo eof; )
}
}
