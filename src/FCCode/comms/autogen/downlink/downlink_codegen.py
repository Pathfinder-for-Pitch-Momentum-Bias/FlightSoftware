import math
import csv

FIELDS = []
with open('downlink_fields.csv') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    for row in csv_reader:
        if line_count != 0:
            field = {"group" : row[0], "name" : row[1], "type" : row[5], "size" : row[6], "buf_size" : row[4], "min" : row[2], "max" : row[3]}
            FIELDS.append(field)
        line_count += 1
for field in FIELDS:
    field["size"] = int(field['size'])
    field["buf_size"] = int(field['buf_size'])
    field["total_size"] = field["size"] * field["buf_size"]


#####################################
# Generates code for the header file containing the packet downlink function definition.
#####################################
def header_generator(fields):
    PACKET_SIZE_BITS = 70 * 8
    FRAME_SIZE_BITS = sum([field["total_size"] for field in fields])

    header_str = "/** THIS IS AN AUTOGENERATED FILE **/\n\n"
    header_str += "#ifndef DOWNLINK_PACKET_GENERATOR_HPP_\n"
    header_str += "#define DOWNLINK_PACKET_GENERATOR_HPP_\n\n"

    header_str += "#include \"../state/state_holder.hpp\"\n"
    header_str += "#include <bitset>" + "\n\n"

    header_str += "namespace Comms {\n"
    header_str += "  constexpr unsigned int PACKET_SIZE_BITS = {0};\n".format(
        PACKET_SIZE_BITS)
    header_str += "  constexpr unsigned int FRAME_SIZE_BITS = {0};\n".format(
        FRAME_SIZE_BITS)
    header_str += "  constexpr unsigned int NUM_PACKETS = (unsigned int) ceil((FRAME_SIZE_BITS + 0.0f) / PACKET_SIZE_BITS);\n"
    header_str += "  void generate_packets(std::bitset<PACKET_SIZE_BITS> (&packets)[NUM_PACKETS], unsigned int downlink_no);\n"
    header_str += "}\n\n"

    header_str += "#endif"

    return header_str

#####################################
# Generates code for a history field of the form
#
# rwMtxRLock(&State::group::lock);
# while(!State::group::val.empty()) {
#   std::bitset<X> representation_i;
#   Comms::trim_float(State::group::val.get(), MIN, MAX, representation_i);
#   for(int i = 0; i < representation_i.size(); i++) {
#     packet.set(packet_ptr++, representation_i[i]);
#   }
# }
# rwMtxRUnlock(&State::group::lock);
#####################################
def generate_history_field_code(field, i, packet_no):
    serializer_code = ""

    lock_name = field["group"] + "::" + field["group"][14:].lower() + "_history_state_lock"
    lock_line = "rwMtxRLock(&{0});\n".format(lock_name)
    serializer_code += lock_line
    while_line = "while(!{0}.empty()) {{\n".format(field["name"])
    serializer_code += while_line

    bitset_name = "bitset_{0}".format(i)
    bitset_definition_line = "  std::bitset<{0}> {1};".format(field["size"],bitset_name)
    serializer_code += bitset_definition_line + "\n"

    serializer_line = ""
    if field["type"] == "bool":
        serializer_line = "  {0}.set(0, {1}.get());".format(bitset_name, field["name"])
    elif field["type"] == "int":
        serializer_line = "  Comms::trim_int({0}.get(), {1}, {2}, &{3});".format(field["name"], field['min'], field['max'], bitset_name)
    elif field["type"] == "temperature":
        serializer_line = "  Comms::trim_temperature({0}.get(), &{1});".format(field["name"], bitset_name)
    elif field["type"] == "float":
        serializer_line = "  Comms::trim_float({0}.get(), {1}, {2}, &{3});".format(field["name"], field['min'], field['max'], bitset_name)
    elif field["type"] == "double":
        serializer_line = "  Comms::trim_double({0}.get(), {1}, {2}, &{3});".format(field["name"], field['min'], field['max'], bitset_name)
    elif field["type"] == "float vector" or field["type"] == "double vector":
        serializer_line = "  Comms::trim_vector({0}.get(), {1}, {2}, &{3});".format(field["name"], field['min'], field['max'], bitset_name)
    elif field["type"] == "quaternion":
        serializer_line = "  Comms::trim_quaternion({0}.get(), &{1});".format(field["name"], bitset_name)
    elif field["type"] == "gps time":
        serializer_line = "  Comms::trim_gps_time({0}.get(), &{1});".format(field["name"], bitset_name)
    else:
        err = "Field {0} does not have a valid type".format(field["name"])
        print(err)
    serializer_code += serializer_line + "\n"

    add_to_packet_line = "  for(int i = 0; i < {0}.size(); i++) packet.set(packet_ptr++,{0}[i]);".format(bitset_name)
    serializer_code += add_to_packet_line
    serializer_code += "}\n"
    unlock_line = "rwMtxRUnlock(&{0});\n".format(lock_name)
    serializer_code += unlock_line
    return serializer_code

#####################################
# Generates code for a non-historical field of the form

# std::bitset<X> representation_i;
# Comms::trim_float(State::read(State::group::val, State::group::lock), MIN, MAX, representation_i);
# for(int i = 0; i < representation_i.size(); i++) {
#   packet.set(packet_ptr++, representation_i[i]);
# }
#####################################
def generate_field_code(field, i, packet_no):
    bitset_name = "bitset_{0}".format(i)
    bitset_definition = "std::bitset<{0}> {1};".format(field["size"],bitset_name)

    serializer_line = ""
    lock_name = field["group"] + "::" + field["group"][7:].lower() + "_state_lock"
    if field["type"] == "bool":
        serializer_line = "{0}.set(0, State::read({1}, {2}));".format(bitset_name, field["name"], lock_name)
    elif field["type"] == "int":
        serializer_line = "Comms::trim_int(State::read({0},{1}), {2}, {3}, &{4});".format(field["name"], lock_name, field['min'], field['max'], bitset_name)
    elif field["type"] == "temperature":
        serializer_line = "Comms::trim_temperature(State::read({0},{1}), &{2});".format(field["name"], lock_name, bitset_name)
    elif field["type"] == "float":
        serializer_line = "Comms::trim_float(State::read({0},{1}), {2}, {3}, &{4});".format(field["name"], lock_name, field['min'], field['max'], bitset_name)
    elif field["type"] == "double":
        serializer_line = "Comms::trim_double(State::read({0},{1}), {2}, {3}, &{4});".format(field["name"], lock_name, field['min'], field['max'], bitset_name)
    elif field["type"] == "float vector" or field["type"] == "double vector":
        serializer_line = "Comms::trim_vector(State::read({0},{1}), {2}, {3}, &{4});".format(field["name"], lock_name, field['min'], field['max'], bitset_name)
    elif field["type"] == "quaternion":
        serializer_line = "Comms::trim_quaternion(State::read({0},{1}), &{2});".format(field["name"], lock_name, bitset_name)
    elif field["type"] == "gps time":
        serializer_line = "Comms::trim_gps_time(State::read({0},{1}), &{2});".format(field["name"], lock_name, bitset_name)
    else:
        err = "Field {0} does not have a valid type".format(field["name"])
        print(err)

    epilogue =   "for(int i = 0; i < {0}.size(); i++) packet.set(packet_ptr++,{0}[i]);".format(bitset_name)

    serializer_code =   bitset_definition + "\n" \
          + serializer_line + "\n" \
          + epilogue + "\n"
    return serializer_code

#####################################
# Generates code for all packet generator functions
#####################################
def packet_fn_generator(fields):
    PACKET_SIZE_BITS = 70*8
    FRAME_SIZE_BITS = sum([field["total_size"] for field in fields])
    NUM_PACKETS = int(math.ceil((FRAME_SIZE_BITS + 0.0) / PACKET_SIZE_BITS))

    packet_generator_str = ""

    field_ptr = 0
    for packet_no in range(0, NUM_PACKETS):
        current_packet_size = 0

        packet_str  = "static void generate_packet_{0}(std::bitset<Comms::PACKET_SIZE_BITS> &packet, unsigned int downlink_no) {{\n".format(packet_no)
        packet_str += "  unsigned int packet_ptr;\n\n"
        packet_str += "  std::bitset<32> downlink_num_repr(downlink_no);\n"
        packet_str += "  for(int i = 0; i < 32; i++) packet.set(packet_ptr++, downlink_num_repr[i]);\n"
        current_packet_size += 32

        packet_str += "  std::bitset<8> packet_num_repr({0});".format(packet_no) + "\n"
        packet_str += "  for(int i = 0; i < 8; i++) packet.set(packet_ptr++, packet_num_repr[i]);\n"
        current_packet_size += 8

        for x in range(field_ptr, len(fields)):
            field = fields[x]
            if current_packet_size + field["total_size"] > PACKET_SIZE_BITS:
                break

            if "history" in field["name"]:
                field_code = generate_history_field_code(field, field_ptr, packet_no)
            else:
                field_code = generate_field_code(field, field_ptr, packet_no)
            field_code_tabbed = "\n".join(["  " + line for line in field_code.splitlines()])
            packet_str += "\n" + field_code_tabbed + "\n"

            current_packet_size += field["total_size"]
            field_ptr += 1

        packet_str += "}\n\n"
        packet_generator_str += packet_str

    return packet_generator_str

def frame_generator(fields):
    PACKET_SIZE_BITS = 70 * 8
    FRAME_SIZE_BITS = sum([field["total_size"] for field in fields])
    NUM_PACKETS = int(math.ceil((FRAME_SIZE_BITS + 0.0) / PACKET_SIZE_BITS))

    frame_str = "/** THIS IS AN AUTOGENERATED FILE **/\n\n"
    frame_str += "#include \"downlink_packet_generator.hpp\"\n"
    frame_str += "#include \"../state/state_holder.hpp\"\n"
    frame_str += "#include \"../state/state_history_holder.hpp\"\n"
    frame_str += "#include <comms_utils.hpp>\n\n"
    frame_str += packet_fn_generator(fields)
    frame_str += "void Comms::generate_packets(std::bitset<Comms::PACKET_SIZE_BITS> (&packets)[Comms::NUM_PACKETS], unsigned int downlink_no) {\n"
    for x in range(0, NUM_PACKETS):
        frame_str += "  generate_packet_{0}(packets[{0}], downlink_no);\n".format(x)
    frame_str += "}\n"

    return frame_str

header_file_content = header_generator(FIELDS)
f = open("../../downlink_packet_generator.hpp", "w")
f.write(header_file_content)
f.close()

implementation_content = frame_generator(FIELDS)
f = open("../../downlink_packet_generator.cpp", "w")
f.write(implementation_content)
f.close()