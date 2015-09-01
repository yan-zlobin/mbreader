#!/bin/sh

DIR="../types"
PREFIX="mb_marshal"

echo "Creating GObject marshallers..."

glib-genmarshal --prefix ${PREFIX} --header marshaller.list > ${DIR}/mb-marshaller.h
glib-genmarshal --prefix ${PREFIX} --body marshaller.list > ${DIR}/mb-marshaller.c

echo "Done"

exit 0


