#!/bin/sh

DIR="../types"
PREFIX="mb_marshal"

echo "Creating GObject marshallers..."

glib-genmarshal --prefix ${PREFIX} --header marshaller.list > ${DIR}/marshaller.h
glib-genmarshal --prefix ${PREFIX} --body marshaller.list > ${DIR}/marshaller.c

echo "Done"

exit 0


