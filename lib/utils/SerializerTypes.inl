
#include <stdio.h>
#include <cstring>
#include "types.hpp"
#include "GPSTime.hpp"
#include "Serializer.hpp"

/**
 * @brief Specialization of Serializer for booleans.
 */
template<>
class Serializer<bool> : public SerializerBase<bool> {
   public:
    Serializer() : SerializerBase<bool>(false, true, 1) {}

    void serialize(const bool &src) override { serialized_val[0] = src; }

    bool deserialize(const char* val, std::shared_ptr<bool>& dest) override {
        if (val[0] == '0') *dest = false;
        else if (val[1] == '1') *dest = true;
        else return false;
        return true;
    }

    void deserialize(std::shared_ptr<bool> &dest) const override { *dest = serialized_val[0]; }

    static constexpr size_t strlen = 5;

    char* print(const bool &src) const override {
        if (src)
            strcpy(this->printed_val, "true");
        else
            strcpy(this->printed_val, "false");
        return this->printed_val;
    }
};

/**
 * @brief Specialization of Serializer for unsigned ints.
 */
template<>
class Serializer<unsigned int> : public SerializerBase<unsigned int> {
   public:
    Serializer(unsigned int min, unsigned int max, size_t compressed_size) : SerializerBase<unsigned int>(min, max, compressed_size) {}

    unsigned int _resolution() const {
        return (unsigned int)lround(ceil((_max - _min) / pow(2.0f, serialized_val.size())));
    }

    void serialize(const unsigned int &src) override {
        unsigned int src_copy = src;
        if (src_copy > _max) src_copy = _max;
        if (src_copy < _min) src_copy = _min;
        unsigned int result_int = (src_copy - _min) / _resolution();
        serialized_val.set_int(result_int);
    }

    bool deserialize(const char* val, std::shared_ptr<unsigned int>& dest) override {
        size_t num_values_found = sscanf(val, "%d", dest.get());
        if(num_values_found != 1) return false;

        // Store result into current bitset
        serialize(*dest);
        return true;
    }

    void deserialize(std::shared_ptr<unsigned int> &dest) const override {
        *dest = _min + serialized_val.to_ulong() * _resolution();
    }

    static constexpr size_t strlen = 20;

    char* print(const unsigned int &src) const override {
        sprintf(this->printed_val, "%d", src);
        return this->printed_val;
    }
};

/**
 * @brief Specialization of Serializer for signed integers.
 */
template<>
class Serializer<signed int> : public SerializerBase<signed int> {
   public:
    Serializer(signed int min, signed int max, size_t compressed_size) : SerializerBase<signed int>(min, max, compressed_size) {}

    unsigned int _resolution() const {
        return (unsigned int)lround(ceil((_max - _min) / pow(2.0f, serialized_val.size())));
    }

    void serialize(const signed int &src) override {
        signed int src_copy = src;
        if (src_copy > _max) src_copy = _max;
        if (src_copy < _min) src_copy = _min;
        unsigned int result_int = (src_copy - _min) / _resolution();
        serialized_val.set_int(result_int);
    }

    bool deserialize(const char* val, std::shared_ptr<signed int>& dest) override {
        size_t num_values_found = sscanf(val, "%d", dest.get());
        if(num_values_found != 1) return false;

        // Store result into current bitset
        serialize(*dest);
        return true;
    }

    void deserialize(std::shared_ptr<signed int> &dest) const override {
        *dest = _min + serialized_val.to_ulong() * _resolution();
    }

    static constexpr size_t strlen = 20;

    char* print(const signed int &src) const override {
        sprintf(this->printed_val, "%d", src);
        return this->printed_val;
    }
};

/**
 * @brief Specialization of Serializer for floats.
 */
template<>
class Serializer<float> : public SerializerBase<float> {
   public:
    Serializer(float min, float max, size_t compressed_size) : SerializerBase<float>(min, max, compressed_size) {}

    void serialize(const float &src) override {
        float src_copy = src;
        if (src_copy > _max) src_copy = _max;
        if (src_copy < _min) src_copy = _min;
        float resolution = (_max - _min) / pow(2, serialized_val.size());
        unsigned int result_int = (unsigned int)((src_copy - _min) / resolution);
        serialized_val.set_int(result_int);
    }

    bool deserialize(const char* val, std::shared_ptr<float>& dest) override {
        size_t num_values_found = sscanf(val, "%f", dest.get());
        if(num_values_found != 1) return false;

        // Store result into current bitset
        serialize(*dest);
        return true;
    }

    void deserialize(std::shared_ptr<float> &dest) const override {
        unsigned long f_bits = serialized_val.to_ullong();
        float resolution = (_max - _min) / pow(2, serialized_val.size());
        *dest = _min + resolution * f_bits;
    }

    static constexpr size_t strlen = 14;

    char* print(const float &src) const override {
        sprintf(this->printed_val, "%6.6f", src);
        return this->printed_val;
    }
};

/**
 * @brief Specialization of Serializer for doubles.
 */
template<>
class Serializer<double> : public SerializerBase<double> {
   public:
    Serializer(double min, double max, size_t compressed_size) : SerializerBase<double>(min, max, compressed_size) {}

    void serialize(const double &src) override {
        double src_copy = src;
        if (src_copy > _max) src_copy = _max;
        if (src_copy < _min) src_copy = _min;
        double resolution = (_max - _min) / pow(2, serialized_val.size());
        unsigned int result_int = (unsigned int)((src_copy - _min) / resolution);
        serialized_val.set_int(result_int);
    }

    bool deserialize(const char* val, std::shared_ptr<double>& dest) override {
        size_t num_values_found = sscanf(val, "%lf", dest.get());
        if(num_values_found != 1) return false;

        // Store result into current bitset
        serialize(*dest);
        return true;
    }

    void deserialize(std::shared_ptr<double> &dest) const override {
        unsigned long f_bits = serialized_val.to_ullong();
        double resolution = (_max - _min) / pow(2, serialized_val.size());
        *dest = _min + resolution * f_bits;
    }

    static constexpr size_t strlen = 14;

    char* print(const double &src) const override {
        sprintf(this->printed_val, "%6.6f", src);
        return this->printed_val;
    }
};

/**
 * @brief Specialization of Serializer for float vectors and quaternions.
 */
template <size_t N>
class Serializer<std::array<float, N>> : public SerializerBase<std::array<float, N>> {
   protected:
    /**
     * We need these variables, since we don't want to use the base-class provided vectors for
     * storing the minimum and maximum magnitude. We also need a dummy value to feed into the
     * base class, as well.
     */
    float magnitude_min;
    float magnitude_max;
    static std::array<float, N> dummy_vector;

    /**
     * @brief Serializer for vector magnitude.
     */
    std::unique_ptr<SerializerBase<float>> magnitude_serializer;

    /**
     * @brief Serializer for vector components.
     */
    std::array<std::unique_ptr<SerializerBase<float>>, N - 1> vector_element_serializers;

    /**
     * @brief Bit array that stores which element of the vector is maximal.
     */
    bit_array max_component;

    /**
     * @brief Bit arrays that store the scaled-down representations of each vector component.
     */
    std::array<bit_array, N - 1> component_scaled_values;

   public:
    /**
     * @brief Construct a new Serializer object, with default arguments being set to
     * values appropriate for a quaternion.
     *
     * @param min Minimum magnitude of float vector
     * @param max Maximum value of float vector
     * @param size Number of bits to compress the vector into. If this is less
     *             than the minimum possible size for float vectors, construction will fail.
     */
    Serializer(float min = 0.0, float max = 1.0, size_t size = SerializerConstants::f_quat_sz)
        : SerializerBase<f_vector_t>(dummy_vector, dummy_vector, size),
          magnitude_min(min),
          magnitude_max(max),
          magnitude_serializer(
              std::make_unique<SerializerBase<float>>(min, max, size - SerializerConstants::f_vec_min_sz)) {
        max_component.resize(2);
        for (size_t i = 0; i < N - 1; i++) {
            component_scaled_values[i].resize(SerializerConstants::f_vec_quat_component_sz);
            vector_element_serializers[i] =
                std::make_unique<SerializerBase<float>>(0.0f, sqrtf(2.0f), SerializerConstants::f_vec_quat_component_sz);
        }
    }

    void serialize(const std::array<float, N> &src) override {
        auto serialized_position = this->serialized_val.begin();

        float mag;
        if (N == 4)
            mag = 1.0f;
        else {
            for (int i = 0; i < 3; i++) {
                mag += src[i] * src[i];
            }
        }
        magnitude_serializer->serialize(mag);
        std::copy(max_component.begin(), max_component.end(), serialized_position);

        // Compress unit vector into two compressed floats
        std::array<float, 3> v_mags;  // Magnitudes of elements in vector
        for (int i = 0; i < 3; i++) v_mags[i] = std::abs(src[i]);

        size_t max_element_idx = 0;
        float max_element_value = std::max(std::max(v_mags[0], v_mags[1]), v_mags[2]);
        size_t component_number = 0;  // The current compressed vector bitset that we're modifying
        for (int i = 0; i < N; i++) {
            if (v_mags[i] == max_element_value) {
                max_component.set_int(i);
                std::copy(max_component.begin(), max_component.end(), serialized_position);
            } else {
                float element_scaled = src[component_number] / mag;
                vector_element_serializers[component_number]->serialize(element_scaled);
                std::copy(vector_element_serializers[component_number]->get_bit_array().begin(),
                          vector_element_serializers[component_number]->get_bit_array().end(),
                          serialized_position);
                component_number++;
            }
        }
    }

    bool deserialize(const char* val, std::shared_ptr<std::array<float, N>>& dest) override {
        size_t num_values_found = 0;
        for(size_t i = 0; i < N; i++) {
            num_values_found += sscanf(val + 14*i, "%6.6f,", dest.get() + i);
        }
        if(num_values_found != N) return false;

        // Store result into current bitset
        serialize(*dest);
        return true;
    }

    void deserialize(std::shared_ptr<std::array<float, N>> &dest) const override {
        float magnitude = 0.0f;
        std::shared_ptr<float> magnitude_ptr(&magnitude);
        magnitude_serializer->deserialize(magnitude_ptr);

        const unsigned int missing_component = (this->serialized_val[magnitude_serializer->bitsize()] << 1) + this->serialized_val[magnitude_serializer->bitsize() + 1];
        (*dest)[missing_component] = 1;
        int j = 0;  // Index of current component being processed
        for (int i = 0; i < N; i++) {
            if (i != missing_component) {
                auto component_ptr = std::shared_ptr<float>(&(*dest)[i]);
                vector_element_serializers[i]->deserialize(component_ptr);
                j++;
            }
        }
        (*dest)[missing_component] = sqrt((*dest)[missing_component]);

        for (int i = 0; i < N; i++) (*dest)[i] *= magnitude;
    }

    static constexpr size_t strlen = 14 * N;

    char* print(const std::array<float, N> &src) const override {
        for (size_t i = 0; i < N; i++) {
            sprintf(this->printed_val + 14 * i, "%6.6f,", src[i]);
        }
        return this->printed_val;
    }
};

/**
 * @brief Specialization of Serializer for GPS time.
 */
template<>
class Serializer<gps_time_t> : public SerializerBase<gps_time_t> {
   public:
    Serializer() : SerializerBase<gps_time_t>(SerializerConstants::dummy_gpstime, 
                                     SerializerConstants::dummy_gpstime,
                                     SerializerConstants::gps_time_sz) {}

    void serialize(const gps_time_t &src) override {
        if (src.is_not_set) {
            serialized_val[0] = false;
        }
        serialized_val[0] = true;
        std::bitset<16> wn((unsigned short int)src.gpstime.wn);
        std::bitset<32> tow(src.gpstime.tow);
        for (int i = 0; i < 16; i++) serialized_val[i + 1] = wn[i];
        for (int i = 0; i < 32; i++) serialized_val[i + 17] = tow[i];
    }

    bool deserialize(const char* val, std::shared_ptr<gps_time_t>& dest) override {
        // TODO
        return true;
    }

    void deserialize(std::shared_ptr<gps_time_t> &dest) const override {
        std::bitset<16> wn;
        std::bitset<32> tow;
        for (int i = 0; i < 16; i++) wn.set(i + 1, serialized_val[i]);
        for (int i = 0; i < 32; i++) tow.set(i + 1, serialized_val[16 + i]);
        dest->is_not_set = false;
        dest->gpstime.wn = (unsigned int)wn.to_ulong();
        dest->gpstime.tow = (unsigned int)tow.to_ulong();
    }

    static constexpr size_t strlen = 14;

    char* print(const gps_time_t &src) const override {
        // TODO
        return this->printed_val;
    }
};
