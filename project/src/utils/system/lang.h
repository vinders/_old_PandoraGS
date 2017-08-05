/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : language-specific resources
*******************************************************************************/
#pragma once

#include <cstdint>

/// @namespace utils
/// General utilities
namespace utils
{
    /// @namespace utils.system
    /// System management utilities
    namespace system
    {
        /// @enum language_t
        /// @brief Languages
        enum class language_t : uint32_t
        {
            other = 0,
            ar_sa = 0x0401,
            bg_bg = 0x0402,
            bo_cn = 0x0451,
            cs_cz = 0x0405,
            da_dk = 0x0406,
            de_de = 0x0407,
            en_gb = 0x0809,
            en_us = 0x0409,
            et_ee = 0x0425,
            es_es = 0x0C0A,
            es_mx = 0x080A,
            fa_ir = 0x0429,
            fi_fi = 0x040B,
            fr_fr = 0x040C,
            he_il = 0x040D,
            hi_in = 0x0439,
            hr_hr = 0x041A,
            hu_hu = 0x040E,
            id_id = 0x0421,
            is_is = 0x040F,
            it_it = 0x0410,
            ja_jp = 0x0411,
            ko_kr = 0x0412,
            lt_lt = 0x0427,
            lb_lu = 0x046E,
            mk_mk = 0x042F,
            mn_mn = 0x0450,
            nb_no = 0x0414,
            ne_np = 0x0461,
            nn_no = 0x0814,
            nl_nl = 0x0413,
            pl_pl = 0x0415,
            pt_br = 0x0416,
            pt_pt = 0x0816,
            ro_ro = 0x0418,
            ru_ru = 0x0419,
            sk_sk = 0x041B,
            sl_si = 0x0424,
            so_so = 0x0477,
            sv_se = 0x041D,
            tn_bw = 0x0832,
            tn_za = 0x0432,
            tr_tr = 0x041F,
            vi_vn = 0x042A,
            xh_za = 0x0434,
            zh_cn = 0x0804
        };
        
        /// @class ILanguageData
        /// @brief Language-specific data container - interface
        class ILanguageData
        {
        public:
            /// @brief Set language-specific data
            template <utils::system::language_t Lang>
            void setLanguage();
        };
    }
}
