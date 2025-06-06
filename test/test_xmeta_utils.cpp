/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <variant>

#include "xtl/xmeta_utils.hpp"

#include "test_common_macros.hpp"

namespace xtl
{
    using vector_t = mpl::vector<int, double, char*>;
    using variant_t = std::variant<int, double, char*>;

    TEST(mpl, if_)
    {
        using true_cond = mpl::bool_<true>;
        using false_cond = mpl::bool_<false>;

        bool res1 = std::is_same<mpl::if_t<true_cond, vector_t, variant_t>, vector_t>::value;
        bool res2 = std::is_same<mpl::if_t<false_cond, vector_t, variant_t>, variant_t>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
    }

    TEST(mpl, cast)
    {
        using type = mpl::cast_t<vector_t, std::variant>;
        bool res = std::is_same<type, variant_t>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, size)
    {
        std::size_t size = mpl::size<vector_t>::value;
        EXPECT_EQ(size, 3u);
    }

    TEST(mpl, empty)
    {
        bool res1 = mpl::empty<vector_t>::value;
        bool res2 = mpl::empty<mpl::vector<>>::value;

        EXPECT_FALSE(res1);
        EXPECT_TRUE(res2);
    }

    TEST(mpl, count)
    {
        using type = mpl::vector<int, double, int, float, double, double>;
        std::size_t c1 = mpl::count<type, int>::value;
        std::size_t c2 = mpl::count<type, double>::value;
        std::size_t c3 = mpl::count<type, float>::value;
        std::size_t c4 = mpl::count<type, char>::value;

        EXPECT_EQ(c1, 2u);
        EXPECT_EQ(c2, 3u);
        EXPECT_EQ(c3, 1u);
        EXPECT_EQ(c4, 0u);
    }

    template <class T>
    struct is_double : std::is_same<T, double>
    {
    };

    TEST(mpl, count_if)
    {
        using type = mpl::vector<int, double, float, double>;
        std::size_t c1 = mpl::count_if<type, is_double>::value;
        EXPECT_EQ(c1, 2u);
    }

    TEST(mpl, contains)
    {
        using type = mpl::vector<int, double, double>;
        bool res1 = mpl::contains<type, int>::value;
        bool res2 = mpl::contains<type, double>::value;
        bool res3 = mpl::contains<type, float>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
        EXPECT_FALSE(res3);
    }

    TEST(mpl, index_of)
    {
        using type1 = mpl::vector<int, double, float>;
        std::size_t res1 = mpl::index_of<type1, int>::value;
        std::size_t res2 = mpl::index_of<type1, double>::value;
        std::size_t res3 = mpl::index_of<type1, float>::value;
        std::size_t res4 = mpl::index_of<type1, short>::value;

        EXPECT_EQ(res1, 0u);
        EXPECT_EQ(res2, 1u);
        EXPECT_EQ(res3, 2u);
        EXPECT_EQ(res4, SIZE_MAX);
    }

    TEST(mpl, front)
    {
        using type = mpl::front_t<variant_t>;
        bool res = std::is_same<type, int>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, back)
    {
        using type = mpl::back_t<variant_t>;
        bool res = std::is_same<type, char*>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, push_front)
    {
        using type = mpl::push_front_t<vector_t, double>;
        using res_type = mpl::vector<double, int, double, char*>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, push_back)
    {
        using type = mpl::push_back_t<vector_t, double>;
        using res_type = mpl::vector<int, double, char*, double>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, pop_front)
    {
        using type = mpl::pop_front_t<vector_t>;
        using res_type = mpl::vector<double, char*>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, transform)
    {
        using type = mpl::transform_t<std::add_pointer_t, vector_t>;
        using res_type = mpl::vector<int*, double*, char**>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, merge_set)
    {
        using arg2 = mpl::vector<double, float, int, short>;
        using type = mpl::merge_set_t<vector_t, arg2>;
        using res_type = mpl::vector<int, double, char*, float, short>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);

        using res_type2 = mpl::merge_set_t<vector_t, vector_t>;
        bool res2 = std::is_same<res_type2, vector_t>::value;
        EXPECT_TRUE(res2);
    }

    TEST(mpl, find_if_)
    {
        using arg1 = mpl::vector<double, int, float, int>;
        constexpr size_t res1 = mpl::find_if<xtl::is_integral, arg1>::value;
        EXPECT_EQ(res1, 1ul);

        using arg2 = mpl::vector<double, float, double>;
        constexpr size_t res2 = mpl::find_if<xtl::is_integral, arg2>::value;
        EXPECT_EQ(res2, 3ul);

        using arg3 = mpl::vector<>;
        constexpr size_t res3 = mpl::find_if<xtl::is_integral, arg3>::value;
        EXPECT_EQ(res3, 0ul);
    }

    TEST(mpl, split)
    {
        using arg = mpl::vector<double, int, float, short>;
        {
            using sp = mpl::split<0u, arg>;
            using first_res_type = typename sp::first_type;
            using second_res_type = typename sp::second_type;
            bool first_res = std::is_same<first_res_type, mpl::vector<>>::value;
            bool second_res = std::is_same<second_res_type, arg>::value;
            EXPECT_TRUE(first_res);
            EXPECT_TRUE(second_res);
        }
        {
            using sp = mpl::split<1u, arg>;
            using first_res_type = typename sp::first_type;
            using second_res_type = typename sp::second_type;
            bool first_res = std::is_same<first_res_type, mpl::vector<double>>::value;
            bool second_res = std::is_same<second_res_type, mpl::vector<int, float, short>>::value;
            EXPECT_TRUE(first_res);
            EXPECT_TRUE(second_res);
        }
        {
            using sp = mpl::split<2u, arg>;
            using first_res_type = typename sp::first_type;
            using second_res_type = typename sp::second_type;
            bool first_res = std::is_same<first_res_type, mpl::vector<double, int>>::value;
            bool second_res = std::is_same<second_res_type, mpl::vector<float, short>>::value;
            EXPECT_TRUE(first_res);
            EXPECT_TRUE(second_res);
        }
        {
            using sp = mpl::split<3u, arg>;
            using first_res_type = typename sp::first_type;
            using second_res_type = typename sp::second_type;
            bool first_res = std::is_same<first_res_type, mpl::vector<double, int, float>>::value;
            bool second_res = std::is_same<second_res_type, mpl::vector<short>>::value;
            EXPECT_TRUE(first_res);
            EXPECT_TRUE(second_res);
        }
        {
            using sp = mpl::split<4u, arg>;
            using first_res_type = typename sp::first_type;
            using second_res_type = typename sp::second_type;
            bool first_res = std::is_same<first_res_type, arg>::value;
            bool second_res = std::is_same<second_res_type, mpl::vector<>>::value;
            EXPECT_TRUE(first_res);
            EXPECT_TRUE(second_res);
        }
    }

    TEST(mpl, unique)
    {
        using input1 = mpl::vector<double, int, double, int, int>;
        using input2 = mpl::vector<double, int>;
        using res_type1 = mpl::unique_t<input1>;
        using res_type2 = mpl::unique_t<input2>;

        constexpr bool res1 = std::is_same<res_type1, input2>::value;
        constexpr bool res2 = std::is_same<res_type2, input2>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
    }

    template <class T>
    using switch_type = mpl::switch_t<std::is_integral<T>, int,
                                      std::is_floating_point<T>, double,
                                      mpl::default_t, char>;

    TEST(mpl, switch_t)
    {
        constexpr bool res1 = std::is_same<switch_type<short>, int>::value;
        constexpr bool res2 = std::is_same<switch_type<float>, double>::value;
        constexpr bool res3 = std::is_same<switch_type<mpl::vector<>>, char>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
        EXPECT_TRUE(res3);
    }
}
