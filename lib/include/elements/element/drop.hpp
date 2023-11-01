/*=============================================================================
   Copyright (c) 2016-2020 Joel de Guzman

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(ELEMENTS_DROP_OCOBER_11_2023)
#define ELEMENTS_DROP_OCOBER_11_2023

#include <elements/element/proxy.hpp>
#include <functional>
#include <set>

namespace cycfi { namespace elements
{
   template <typename Subject>
   class drop_box : public proxy<Subject>
   {
   public:

      using base_type = proxy<Subject>;
      using on_drop_function = std::function<bool(drop_info const& info)>;

                              drop_box(Subject subject, std::initializer_list<std::string> mime_types_);
      void                    draw(context const& ctx) override;
      bool                    wants_control() const override;

      void                    track_drop(context const& ctx, drop_info const& info, cursor_tracking status) override;
      bool                    drop(context const& ctx, drop_info const& info) override;

      on_drop_function        on_drop = [](drop_info const&){ return false; };

   private:

      using mime_types = std::set<std::string>;

      bool                    _is_tracking = false;
      mime_types              _mime_types;
   };

   template <typename Subject>
   drop_box<Subject>::drop_box(Subject subject, std::initializer_list<std::string> mime_types_)
    : proxy<Subject>{std::move(subject)}
    , _mime_types{mime_types_}
   {}

   template <typename Subject>
   inline void drop_box<Subject>::draw(context const& ctx)
   {
      proxy<Subject>::draw(ctx);
      if (_is_tracking)
      {
         auto& cnv = ctx.canvas;
         auto& bounds = ctx.bounds;
         cnv.stroke_style(get_theme().indicator_hilite_color.opacity(0.5));
         cnv.line_width(2.0);
         cnv.add_rect(bounds);
         cnv.stroke();
      }
   }

   template <typename Subject>
   inline bool drop_box<Subject>::wants_control() const
   {
      return true;
   }

   template <typename Subject>
   inline void drop_box<Subject>::track_drop(context const& ctx, drop_info const& info, cursor_tracking status)
   {
      for (auto item : _mime_types)
      {
         if (info.data.find(item) != info.data.end())
            break;
         return; // Return early if the mime types registered are not in the `drop_info`
      }

      auto new_is_tracking = status != cursor_tracking::leaving;
      if (new_is_tracking != _is_tracking)
      {
         _is_tracking = new_is_tracking;
         ctx.view.refresh(ctx.bounds);
      }
   }

   template <typename Subject>
   inline bool drop_box<Subject>::drop(context const& ctx, drop_info const& info)
   {
      bool r = on_drop(info);
      _is_tracking = false;
      ctx.view.refresh(ctx.bounds);
      return r;
   }
}}

#endif
