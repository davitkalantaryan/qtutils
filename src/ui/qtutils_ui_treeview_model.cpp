//
// file:            qtutils_ui_treeview_model.hpp
// path:			src/ui/qtutils_ui_treeview_model.hpp
// created on:		2022 Jan 21
// created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
//

#include <qtutils/ui/treeview_model.hpp>
#include <cpputils/hashtbl.hpp>
#include <assert.h>
#include <qtutils/disable_utils_warnings.h>


namespace qtutils { namespace ui{ namespace treeview{


namespace model{

class QTUTILS_EXPORT Key
{
public:
    Key(const model::Node* a_pParent, int a_row, int a_col);
    bool operator==(const Key&)const;
public:
    const model::Node*const    m_pParent;
    const int                  m_row, m_col;
};

class CPPUTILS_DLL_PRIVATE Node_p
{
public:
    Node_p(const Key& a_key);
public:
    const Key   m_key;
};

}  // namespace model{


struct CPPUTILS_DLL_PRIVATE FHash{
    size_t	operator()(const model::Key& a_key)const;
};

typedef ::cpputils::hashtbl::Base<model::Key,const model::Node*,FHash> HashNodes;

class CPPUTILS_DLL_PRIVATE Model_p
{
public:
    Model_p(Model* a_pParent);
    inline const model::Node* getNode(const QModelIndex& a_index)const;
    inline const model::Node* checkNodeAndReplaceIfNeeded(const model::Node* a_pInitData)const;
    inline HashNodes::iterator createNodeIterator(const model::Key& a_index, size_t a_hash)const;
    inline HashNodes::iterator createRootIterator(const model::Key& a_index, size_t a_hash)const;
    inline const model::Node* createNode(const model::Key& a_index)const;
    
private:
    inline void ResetModel();
    
private:
    Model*const         m_pParent;
public:
    uint64_t            m_nIteration;
    mutable HashNodes   m_nodes;
};


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Model::~Model()
{
    delete m_model_data_p;
}


uint64_t Model::iteration()const
{
    return m_model_data_p->m_nIteration;
}


Model_p* Model::newPrivateData(Model* a_pParent)
{
    return new Model_p(a_pParent);
}


const model::Node* Model::replaceNode(const model::Node* a_pInitialNode)const
{
    return m_model_data_p->createNode(a_pInitialNode->m_node_data_p->m_key);
}


int Model::columnCount(const QModelIndex& a_index) const
{
    if(a_index.isValid()){
        const model::Node* pNode = m_model_data_p->getNode(a_index);
        assert(pNode);
        return pNode->columnCount();
    }
    return rootColumnCount();
}


int Model::rowCount(const QModelIndex& a_index) const
{
    if(a_index.isValid()){
        const model::Node* pNode = m_model_data_p->getNode(a_index);
        assert(pNode);
        return pNode->rowCount();
    }
    return rootRowCount();
}


QVariant Model::data(const QModelIndex& a_index, int a_role) const
{
    assert(a_index.isValid());
    const model::Node* pNode = m_model_data_p->getNode(a_index);
    assert(pNode);
    const QVariant qvRet = pNode->data(a_role);
    //QtUtilsDebug()<<qvRet;
    return qvRet;
}


QModelIndex	Model::index(int a_row, int a_col, const QModelIndex& a_parent) const
{
    if(a_parent.isValid()){
        const model::Node* pParentNode = static_cast<model::Node*>(a_parent.internalPointer());
        const model::Key aKey(pParentNode,a_row,a_col);
        size_t unHash;
        HashNodes::iterator iter = m_model_data_p->m_nodes.FindEntry(aKey,&unHash);
        if(iter==HashNodes::s_endIter){
            iter = m_model_data_p->createNodeIterator(aKey,unHash);
        }
        else{
            iter->second = m_model_data_p->checkNodeAndReplaceIfNeeded(iter->second);
        }
        return createIndex(a_row,a_col,const_cast<model::Node*>(iter->second));
    }
    
    CPPUTILS_DO_DEBUG_EXP( const int cnNumberOfRows(rootRowCount()) )
    CPPUTILS_DO_DEBUG_EXP( const int cnNumberOfCols(rootColumnCount()) )
    assert((a_row<cnNumberOfRows)&&(a_col<cnNumberOfCols));
    
    const model::Key aKey(nullptr,a_row,a_col);
    size_t unHash;
    HashNodes::iterator iter = m_model_data_p->m_nodes.FindEntry(aKey,&unHash);
    if(iter==HashNodes::s_endIter){
        iter = m_model_data_p->createRootIterator(aKey,unHash);
    }
    else{
        iter->second = m_model_data_p->checkNodeAndReplaceIfNeeded(iter->second);
    }
    //QtUtilsDebug()<<" ";
    return createIndex(a_row,a_col,const_cast<model::Node*>(iter->second));
}


QModelIndex	Model::parent(const QModelIndex& a_index) const
{
    assert(a_index.isValid());
    const model::Node* pNode = m_model_data_p->getNode(a_index);
    
    if(pNode->m_node_data_p->m_key.m_pParent){
        const model::Key& aParentKey = pNode->m_node_data_p->m_key.m_pParent->m_node_data_p->m_key;
        return createIndex(aParentKey.m_row,aParentKey.m_col,const_cast<model::Node*>(pNode->m_node_data_p->m_key.m_pParent));
    }
    return QModelIndex();
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Model_p::Model_p(Model* a_pParent)
    :
      m_pParent(a_pParent),
      m_nIteration(0)
{
    QObject::connect(a_pParent,&QAbstractItemModel::modelAboutToBeReset,m_pParent,[this](){
        ResetModel();
    });
}


inline const model::Node* Model_p::getNode(const QModelIndex& a_index)const
{
    const model::Node* pNode = static_cast<model::Node*>(a_index.internalPointer());
    assert(pNode);
    return checkNodeAndReplaceIfNeeded(pNode);
}


inline const model::Node* Model_p::checkNodeAndReplaceIfNeeded(const model::Node* a_pInitNode)const
{
    //if(a_pInitData&&(a_pInitData->m_nIteration!=m_nIteration)){
    if(a_pInitNode->m_nIteration!=m_nIteration){
        a_pInitNode = m_pParent->replaceNode(a_pInitNode);
    }
    return a_pInitNode;
}


inline void Model_p::ResetModel()
{    
    const model::Node* pNode;
    HashNodes::iterator iterTmp, iter = m_nodes.begin();
    while(iter!=HashNodes::s_endIter){
        iterTmp = iter;
        ++iterTmp;
        pNode = iter->second;
        if(pNode->isPossibleToDelete(m_nIteration)){
            delete pNode;
            m_nodes.RemoveEntry(iter);
        }
        iter=iterTmp;
    }
    ++m_nIteration;
}


inline HashNodes::iterator Model_p::createNodeIterator(const model::Key& a_index, size_t a_hash)const
{
    const model::Node* pChild = createNode(a_index);
    return m_nodes.AddEntryWithKnownHash(a_index,pChild,a_hash);
}


inline const model::Node* Model_p::createNode(const model::Key& a_index)const
{
    const model::Node* pParentBase = a_index.m_pParent;
    if(pParentBase){
        return pParentBase->createChild(a_index.m_row,a_index.m_col);
    }
    return m_pParent->createRootNode(a_index.m_row,a_index.m_col);
}


HashNodes::iterator Model_p::createRootIterator(const model::Key& a_index, size_t a_hash)const
{    
    const model::Node* pNodeRoot = m_pParent->createRootNode(a_index.m_row,a_index.m_col);
    return m_nodes.AddEntryWithKnownHash(a_index,pNodeRoot,a_hash);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

size_t FHash::operator()(const model::Key& a_key)const
{
    return (size_t)((size_t)a_key.m_pParent + (size_t)a_key.m_row + (size_t)a_key.m_col);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

namespace model {


Key::Key(const Node* a_pParent, int a_row, int a_col)
    :
      m_pParent(a_pParent),
      m_row(a_row),
      m_col(a_col)
{
}


bool Key::operator==(const Key& a_aM)const
{
    return (this->m_pParent==a_aM.m_pParent)&&(this->m_row==a_aM.m_row)&&(this->m_col==a_aM.m_col);
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Node::Node(const Node* a_pParent, int a_row, int a_col, uint64_t a_nIteration)
    :
      m_node_data_p(new Node_p(Key(a_pParent,a_row,a_col))),
      m_nIteration(a_nIteration)
{
}


Node::~Node()
{
    delete m_node_data_p;
}


bool Node::isPossibleToDelete(uint64_t a_nIteration)const
{
    static_cast<void>(a_nIteration);
    return false;
}


const Node* Node::parentRaw()const
{
    return m_node_data_p->m_key.m_pParent;
}


int Node::row()const
{
    return m_node_data_p->m_key.m_row;
}


int Node::column() const
{
    return m_node_data_p->m_key.m_col;
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

Node_p::Node_p(const Key& a_key)
    :
      m_key(a_key)
{
}


/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

int NodeChild::rowCount()const
{
    return 0;
}


int NodeChild::columnCount()const
{
    return 0;
}


const Node* NodeChild::createChild(int a_row, int a_col)const
{
    assert(false);
    static_cast<void>(a_row);
    static_cast<void>(a_col);
    return nullptr;
}


}  // namespace model {


}}}  // namespace qtutils { namespace ui{ namespace treeview{
