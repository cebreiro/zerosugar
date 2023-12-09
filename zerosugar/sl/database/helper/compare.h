#pragma once
#include <boost/mysql.hpp>
#include <boost/describe.hpp>
#include <boost/container/static_vector.hpp>
#include "zerosugar/shared/type/concept.h"

namespace zerosugar::sl::db
{
    template<class T,
        class Bd = boost::describe::describe_bases<T, boost::describe::mod_any_access>,
        class Md = boost::describe::describe_members<T, boost::describe::mod_any_access>,
        class En = std::enable_if_t<!std::is_union<T>::value>>
        bool FillDifference(const T& oldOne, const T& newOne, std::ostream& os,
            boost::container::static_vector<boost::mysql::field_view, boost::mp11::mp_size<Md>::value>& fieldViews)
    {
        bool result = false;

        boost::mp11::mp_for_each<Md>([&](auto D)
            {

                const auto& oldValue = oldOne.*D.pointer;
                const auto& newValue = newOne.*D.pointer;

                if (oldValue != newValue)
                {
                    os << std::format("`{}` = ?,", D.name);

                    using member_type = std::remove_cvref_t<decltype(newValue)>;


                    if constexpr (is_optional<member_type>)
                    {
                        if (newValue.has_value())
                        {
                            fieldViews.emplace_back(*newValue);
                        }
                        else
                        {
                            fieldViews.emplace_back(nullptr);
                        }
                    }
                    else
                    {
                        fieldViews.emplace_back(newValue);
                    }

                    result = true;
                }
            });

        return result;
    }
}
