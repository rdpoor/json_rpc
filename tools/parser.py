import json
from cffi import FFI

ffi = FFI()
ffi.cdef("""
// ffi.typeof('timestamp_t').kind = 'primitive'
typedef uint32_t timestamp_t;

// ffi.typeof('button_state_t').kind = 'struct'
typedef struct _button_state {
  timestamp_t timestamp;
  bool is_pressed;
} button_state_t;

// ffi.typeof('light_sensor_t').kind = 'enum'
typedef enum {
  COLOR_RED,
  COLOR_GRN,
  COLOR_BLU
} color_t;

// ffi.typeof('light_sensor_t').kind = 'struct'
typedef struct {
  float value;
  color_t color;
} light_sensor_t;

""")

class JrpcDescriptor(object):

    def __init__(self, ffi):
        self._ffi = ffi

    def ffi(self):
        return self._ffi

    def struct_names(self):
        typedefs = self._ffi.list_types()[0]  # all typedefs
        return [x for x in typedefs if self._ffi.typeof(x).kind == 'struct']

    def message_name(self, typedef_name):
        return typedef_name.removesuffix('_t')

    def dump_descriptor_typedefs(self):
        return """
typedef struct {
    const char *name;
    const char *type;
} slot_descriptor_t;

typedef struct {
    const char *name;
    slot_descriptor_t slots[];
} message_descriptor_t;
"""

    def dump_descriptors(self):
        return ''.join([self.dump_descriptor(name) for name in self.struct_names()])

    def dump_descriptor(self, typedef_name):
        message_name = self.message_name(typedef_name)
        return f"""
static message_descriptor_t s_{message_name} = {{
    .name="{message_name}",
    .slots=
        {self.dump_slot_descriptors(typedef_name)},
        NULL
    }},
}};
"""

    def dump_slot_descriptors(self, typedef_name):
        t = ffi.typeof(typedef_name)
        slots = [f'{{.name="{field_name}", .type="{field.type.cname}"}}' for field_name, field in t.fields]
        return ',\n        '.join(slots)

# def dumper(my_type_string):
#     mytype = ffi.typeof(my_type_string)
#     mytypedict = {}
#     for field_name, field in mytype.fields:
#         mytypedict[field_name] = field.type.cname
#
#     print(json.dumps(mytypedict))

def dumper(my_type_string):
    mytype = ffi.typeof(my_type_string)
    a = [f'{{.name="{field_name}", .type="{field.type.cname}"}}' for field_name, field in mytype.fields]
    print(a)

dumper('button_state_t')
dumper('light_sensor_t')

x0 = ffi.typeof("timestamp_t")    # x0.kind = 'primitive'
x1 = ffi.typeof("button_state_t") # x1.kind = 'struct'
x2 = ffi.typeof("color_t")        # x2.kind = 'enum'
x3 = ffi.typeof("light_sensor_t") # x3.kind = 'struct'

j = JrpcDescriptor(ffi)
print(j.dump_descriptor_typedefs())
print(j.dump_descriptors())
