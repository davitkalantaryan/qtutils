//
// file:            treeview_model.hpp
// path:			include/qtutils/ui/treeview_model.hpp
// created on:		2022 Jan 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#pragma once

#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TREEVIEW_MODEL_HPP
#define QTUTILS_INCLUDE_QTUTILS_UI_TREEVIEW_MODEL_HPP


#include <qtutils/qtutils_internal_header.h>
#include <type_traits>
#include <stdint.h>
#include <qtutils/disable_utils_warnings.h>
#include <QAbstractItemModel>


namespace qtutils { namespace ui{ namespace treeview{

#define QTUTILS_TV_MODEL_DEFAULT_DATA()   QVariant()

class CPPUTILS_DLL_PRIVATE Model_p;

namespace model{
class QTUTILS_EXPORT Node;
}

class QTUTILS_EXPORT Model : public QAbstractItemModel
{
public:
    template<typename... Targs>
	Model(Targs... a_args);
    virtual ~Model() override;
    
    uint64_t iteration()const;
    
private:
    virtual int rootRowCount()const = 0;
    virtual int rootColumnCount()const = 0;
    virtual const model::Node* createRootNode(int row, int column)const =0;  // 
    
private:
    static Model_p* newPrivateData(Model* a_pParent);
    
protected:
    virtual int	        rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int	        columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant    data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QModelIndex	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex	parent(const QModelIndex &index) const override;
    virtual const model::Node* replaceNode(const model::Node* a_pInitialNode)const;
    
private:
    Model_p*const   m_model_data_p;
    friend class Model_p;
		
};


namespace model{

class CPPUTILS_DLL_PRIVATE Node_p;

class QTUTILS_EXPORT Node
{
public:
    virtual ~Node();
    Node(const Node* pParent, int row, int column, uint64_t a_nIteration);
    Node(const Node&)=delete;
    const Node& operator=(const Node&)=delete;
    virtual QVariant data(int role = Qt::DisplayRole) const = 0;
    virtual int rowCount()const =0;
    virtual int columnCount()const =0;
    virtual const Node* createChild(int a_row, int a_col)const = 0;
    virtual bool isPossibleToDelete(uint64_t a_nIteration)const;
    int row()const;
    int column()const;
    const Node* parentRaw()const;
    template<typename ParentType>
    const ParentType* parent()const;
private:
    Node_p*const    m_node_data_p;
public:
    const uint64_t  m_nIteration;
private:
    friend class ::qtutils::ui::treeview::Model;
};


class QTUTILS_EXPORT NodeChild : public Node
{
public:
    using Node::Node;
private:
    int rowCount()const override final;
    int columnCount()const override final;
    const Node* createChild(int a_row, int a_col)const override final;
};


class QTUTILS_EXPORT NodeFieldName final : public NodeChild
{
public:
    NodeFieldName(const Node* a_pParentNode, int a_row, int a_col, uint64_t a_nIteration, const QString& a_fieldName);
private:
    QVariant data(int role = Qt::DisplayRole) const override;
public:
    const QString m_fieldName;
};


template <typename NodeType>
class QTUTILS_EXPORT NodeDummyT : public NodeType
{
    static_assert( ::std::is_base_of<Node,NodeType>() );
public:
    using NodeType::NodeType;
private:
    QVariant data(int role = Qt::DisplayRole) const override final;
};


template <typename NodeType>
class NodeRootT : public NodeType{
public:
    NodeRootT(int row, int column, uint64_t a_nIteration);
};
typedef NodeRootT<Node>  NodeRoot;

typedef NodeDummyT<NodeChild>               NodeDummy;
typedef NodeDummyT<NodeRootT<NodeChild> >   NodeDummyRoot;


} // namespace model{


}}}  // namespace qtutils { namespace ui{ namespace treeview{


#ifndef QTUTILS_INCLUDE_QTUTILS_UI_TREEVIEW_MODEL_IMPL_HPP
#include "treeview_model.impl.hpp"
#endif


#endif  // #ifndef QTUTILS_INCLUDE_QTUTILS_UI_TREEVIEW_MODEL_HPP
