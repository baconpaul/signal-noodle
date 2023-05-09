#include <iostream>
#include <type_traits>

template <size_t bs> struct signalOn
{
    float *data{nullptr};
    signalOn(float *s) : data(s) {}

    inline void streamRecieve(float *f) { memcpy(data, f, bs * sizeof(float)); };
    inline float *streamSend() { return data; }
};

template <typename S, typename T,
          std::enable_if_t<std::is_member_function_pointer<decltype(&S::streamRecieve)>::value,
                           bool> = true>
struct CanRecieveT : std::true_type
{
};

template <typename S,
          std::enable_if_t<std::is_member_function_pointer<decltype(&S::streamSend)>::value, bool> =
              true>
struct CanSend : std::true_type
{
};

template <typename S, typename T,
          std::enable_if_t<std::conjunction_v<CanSend<S>, CanRecieveT<S, T>>, bool> = true>
struct IsSendRecievePair : std::true_type
{
};

template <typename S, typename R, std::enable_if_t<IsSendRecievePair<S, R>::value, bool> = true>
R &operator>>(S &in, R &os)
{
    os.streamRecieve(in.streamSend());
    return os;
}

int main(int argc, char **argv)
{
    static constexpr int bs{32};
    float in[bs], out[bs];
    for (int i = 0; i < bs; ++i)
        in[i] = 0.8f * rand() / RAND_MAX;
    auto sin = signalOn<32>(in);
    auto sout = signalOn<32>(out);
    sin >> sout;

    for (int i = 0; i < bs; ++i)
        std::cout << i << " " << in[i] << " " << out[i] << std::endl;
}
