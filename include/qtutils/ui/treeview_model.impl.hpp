//
// file:            treeview_model.impl.hpp
// path:			include/qtutils/ui/treeview_model.impl.hpp
// created on:		2022 Jan 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TREEVIEW_MODEL_IMPL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_TREEVIEW_MODEL_IMPL_HPP


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TREEVIEW_MODEL_HPP
#include "treeview_model.hpp"
#endif


namespace qtutils { namespace ui{ namespace treeview{


template<typename... Targs>
Model::Model(Targs... a_args)
	:
	  QAbstractItemModel(a_args...),
      m_model_data_p(newPrivateData(this))
{
}


namespace model {

template<typename ParentType>
const ParentType* Node::parent()const
{
    static_assert( ::std::is_base_of<Node,ParentType>(), "ParentType should be child of Node" );
    //const ParentType* pParent = CPPUTILS_SAFE_CAST(const ParentType*,parentRaw());
    //assert(pParent);
    const ParentType* pParent = dynamic_cast<const ParentType*>(parentRaw());
    return pParent;
}


template<typename RootType>
const RootType* Node::root()const
{
    static_assert( ::std::is_base_of<Node,RootType>() );
    //const RootType* pRoot = CPPUTILS_SAFE_CAST(const RootType*,rootRaw());
    //assert(pRoot);
    const RootType* pRoot = dynamic_cast<const RootType*>(rootRaw());
    return pRoot;
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

template <typename NodeType>
QVariant NodeDummyT<NodeType>::data(int) const
{
    return QTUTILS_TV_MODEL_DEFAULT_DATA();
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

template <typename NodeType>
NodeRootT<NodeType>::NodeRootT(int a_row, int a_col, uint64_t a_nIteration)
    :
      NodeType(nullptr,a_row,a_col,a_nIteration)
{
    static_assert( ::std::is_base_of<Node,NodeType>() );
}


}  // namespace model {


}}}  // namespace qtutils { namespace ui{ namespace treeview{


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_TREEVIEW_MODEL_IMPL_HPP
