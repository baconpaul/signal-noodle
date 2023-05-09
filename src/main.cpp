#include <iostream>
#include <type_traits>

template <typename Fn> struct wrap
{
    Fn &f_;
    wrap(Fn &&f) : f_(f) {}
    float last{0};
    template <typename... Args> void operator()(Args... args)
    {
        f_(last, std::forward<Args>(args)...);
    }
};

template <size_t bs> struct signalOn
{
    float *data{nullptr};
    signalOn(float *s) : data(s) {}

    inline void streamRecieve(float *f) { memcpy(data, f, bs * sizeof(float)); };
    inline float *streamSend() { return data; }
};

template <size_t bs> struct signalWith : signalOn<bs>
{
    float ldata alignas(16)[bs];
    signalWith() : signalOn<bs>(&ldata[0]) {}
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

template <size_t bs> void cube(float const *__restrict in, float *__restrict out)
{
    for (auto i = 0U; i < bs; ++i)
        out[i] = in[i] * in[i] * in[i];
}

template <size_t bs, void (*F)(float const *__restrict, float *__restrict)> struct liftInOut
{
    float data alignas(16)[bs];

    inline float *streamSend() { return &data[0]; }
    inline void streamRecieve(float *inval) { F(inval, data); }
};

int main(int argc, char **argv)
{
    static constexpr int bs{32};
    float in[bs];
    for (int i = 0; i < bs; ++i)
        in[i] = (bs - i) * 1.f / bs;
    auto sin = signalOn<32>(in);
    auto cub = liftInOut<32, cube<32>>();

    auto sout = signalWith<32>();
    auto soutc = signalWith<32>();
    sin >> sout;

    for (int i = 0; i < bs; ++i)
        std::cout << i << " " << in[i] << " " << sout.data[i] << std::endl;

    sin >> cub >> cub >> soutc;

    for (int i = 0; i < bs; ++i)
    {
        auto d = in[i];
        d = d * d * d;
        d = d * d * d;
        std::cout << i << " " << d << " " << soutc.data[i] << std::endl;
    }
}
