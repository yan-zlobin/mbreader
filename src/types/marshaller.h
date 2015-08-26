
#ifndef __mb_marshal_MARSHAL_H__
#define __mb_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:ULONG,OBJECT (marshaller.list:1) */
extern void mb_marshal_VOID__ULONG_OBJECT (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data);

G_END_DECLS

#endif /* __mb_marshal_MARSHAL_H__ */

