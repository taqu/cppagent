#include "tview.h"
#include "md4c.h"
#include <cassert>
#include <memory>
#include <string>

namespace tview
{
namespace
{
    static int32_t MD_SPAN_EMPTY = MD_SPAN_U + 1;

    struct Node
    {
        int32_t block_type_;
        int32_t detail_;
        char mark_;
        int32_t span_type_;
        std::vector<std::shared_ptr<Node>> children_;
        std::string text_;
    };

    struct Context
    {
        std::vector<std::shared_ptr<Node>> nodes_;
    };

    int enter_block(MD_BLOCKTYPE type, void* detail, void* userdata)
    {
        Context* context = (Context*)userdata;
        switch(type) {
        case MD_BLOCK_DOC: {
            assert(context->nodes_.size() <= 0);
            auto doc = std::shared_ptr<Node>(new Node{});
            doc->block_type_ = MD_BLOCK_DOC;
            doc->span_type_ = -1;
            context->nodes_.emplace_back(doc);
        } break;

        case MD_BLOCK_QUOTE:
        case MD_BLOCK_UL:
        case MD_BLOCK_OL:
        case MD_BLOCK_LI: {
            assert(context->nodes_.back()->span_type_ < 0);
            auto p = std::shared_ptr<Node>(new Node{});
            p->block_type_ = type;
            p->span_type_ = -1;
            context->nodes_.back()->children_.push_back(p);
            context->nodes_.emplace_back(p);
        } break;

        case MD_BLOCK_HR: {
            assert(context->nodes_.back()->span_type_ < 0);
            auto hr = std::shared_ptr<Node>(new Node{});
            hr->block_type_ = MD_BLOCK_HR;
            hr->span_type_ = -1;
            context->nodes_.back()->children_.push_back(hr);
        } break;

        case MD_BLOCK_H: {
            assert(context->nodes_.back()->span_type_ < 0);
            auto h = std::shared_ptr<Node>(new Node{});
            h->block_type_ = MD_BLOCK_H;
            h->span_type_ = -1;
            const MD_BLOCK_H_DETAIL* hdetail = (const MD_BLOCK_H_DETAIL*)detail;
            h->detail_ = static_cast<int32_t>(hdetail->level);
            context->nodes_.back()->children_.push_back(h);
            context->nodes_.emplace_back(h);
        } break;

        case MD_BLOCK_CODE:
        case MD_BLOCK_HTML:
        case MD_BLOCK_P:
        case MD_BLOCK_TABLE:
        case MD_BLOCK_THEAD:
        case MD_BLOCK_TBODY:
        case MD_BLOCK_TR:
        case MD_BLOCK_TH:
        case MD_BLOCK_TD: {
            assert(context->nodes_.back()->span_type_ < 0);
            auto p = std::shared_ptr<Node>(new Node{});
            p->block_type_ = type;
            p->span_type_ = -1;
            context->nodes_.back()->children_.push_back(p);
            context->nodes_.emplace_back(p);
        } break;
        }
        return 0;
    }

    int leave_block(MD_BLOCKTYPE type, void* detail, void* userdata)
    {
        Context* context = (Context*)userdata;
        switch(type) {
        case MD_BLOCK_DOC: {
        } break;

        case MD_BLOCK_QUOTE:
        case MD_BLOCK_UL:
        case MD_BLOCK_OL:
        case MD_BLOCK_LI: {
            context->nodes_.pop_back();
        } break;

        case MD_BLOCK_HR: {
        } break;

        case MD_BLOCK_H:
        case MD_BLOCK_CODE:
        case MD_BLOCK_HTML:
        case MD_BLOCK_P:
        case MD_BLOCK_TABLE:
        case MD_BLOCK_THEAD:
        case MD_BLOCK_TBODY:
        case MD_BLOCK_TR:
        case MD_BLOCK_TH:
        case MD_BLOCK_TD: {
            context->nodes_.pop_back();
        } break;
        }
        return 0;
    }

    int enter_span(MD_SPANTYPE type, void* detail, void* userdata)
    {
        Context* context = (Context*)userdata;
        switch(type) {
        case MD_SPAN_EM:
        case MD_SPAN_STRONG:
        case MD_SPAN_A:
        case MD_SPAN_IMG:
        case MD_SPAN_CODE:
        case MD_SPAN_DEL:
        case MD_SPAN_LATEXMATH:
        case MD_SPAN_LATEXMATH_DISPLAY:
        case MD_SPAN_WIKILINK:
        case MD_SPAN_U: {
            auto p = std::shared_ptr<Node>(new Node{});
            p->block_type_ = -1;
            p->span_type_ = type;
            context->nodes_.back()->children_.push_back(p);
            context->nodes_.emplace_back(p);
        } break;
        }
        return 0;
    }

    int leave_span(MD_SPANTYPE type, void* detail, void* userdata)
    {
        Context* context = (Context*)userdata;
        switch(type) {
        case MD_SPAN_EM:
        case MD_SPAN_STRONG:
        case MD_SPAN_A:
        case MD_SPAN_IMG:
        case MD_SPAN_CODE:
        case MD_SPAN_DEL:
        case MD_SPAN_LATEXMATH:
        case MD_SPAN_LATEXMATH_DISPLAY:
        case MD_SPAN_WIKILINK:
        case MD_SPAN_U: {
            context->nodes_.pop_back();
        } break;
        }
        return 0;
    }

    int parse_text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata)
    {
        Context* context = (Context*)userdata;
        switch(type) {
        case MD_TEXT_NORMAL: {
            context->nodes_.back()->text_.append(text, size);
        } break;
        case MD_TEXT_NULLCHAR: {
        } break;
        case MD_TEXT_BR:
        case MD_TEXT_SOFTBR:
        case MD_TEXT_ENTITY:
        case MD_TEXT_CODE:
        case MD_TEXT_HTML:
        case MD_TEXT_LATEXMATH:
        case MD_SPAN_WIKILINK:
        case MD_SPAN_U: {
            context->nodes_.back()->text_.append(text, size);
        } break;
        }
        return 0;
    }

    ftxui::Element add_node(const std::shared_ptr<Node>& node);
    ftxui::Element add_block(const std::shared_ptr<Node>& node);
    ftxui::Element add_span(const std::shared_ptr<Node>& node);

    ftxui::Elements get_children(const std::shared_ptr<Node>& node)
    {
        ftxui::Elements children;
        for(size_t i = 0; i < node->children_.size(); ++i) {
            children.push_back(add_node(node->children_[i]));
        }
        if(!node->text_.empty()){
            children.push_back(ftxui::text(node->text_));
        }
        return children;
    }

    ftxui::Element add_node(const std::shared_ptr<Node>& node)
    {
        if(0<=node->block_type_){
            return add_block(node);
        }else if(0<=node->span_type_){
            return add_span(node);
        }
        return ftxui::emptyElement();
    }

    ftxui::Element add_block(const std::shared_ptr<Node>& node)
    {
        switch(node->block_type_) {
        case MD_BLOCK_DOC: {
            return ftxui::vbox(get_children(node));
        } break;

        case MD_BLOCK_QUOTE:{
            return ftxui::vbox(get_children(node)) | ftxui::border;
        } break;
        case MD_BLOCK_UL:{
            ftxui::Elements tems;
            for(size_t i = 0; i < node->children_.size(); ++i) {
                tems.push_back(ftxui::hbox(ftxui::text(" - "), add_node(node->children_[i])));
            }
            return ftxui::vbox(tems);
        } break;
        case MD_BLOCK_OL:{
            ftxui::Elements tems;
            for(size_t i = 0; i < node->children_.size(); ++i) {
                tems.push_back(ftxui::hbox(ftxui::text(std::to_string(i+1)), add_node(node->children_[i])));
            }
            return ftxui::vbox(tems);
        } break;
        case MD_BLOCK_LI: {
            return ftxui::hbox(get_children(node));
        } break;

        case MD_BLOCK_HR: {
            return ftxui::separator();
        } break;

        case MD_BLOCK_H:{
            ftxui::Element item = ftxui::vbox(get_children(node));
            switch(node->detail_) {
            case 0:
            case 1:
                return item | ftxui::bold;
            case 2:
                return ftxui::underlined(item | ftxui::bold);
            case 3:
                return item | ftxui::bold;
            case 4:
                return ftxui::underlined(item | ftxui::bold);
            case 5:
                return item | ftxui::bold;
            case 6:
                return ftxui::underlined(item | ftxui::bold);
            default:
                return item;
            }
        } break;
        case MD_BLOCK_CODE:
            return ftxui::vbox(get_children(node));
        case MD_BLOCK_HTML:
            return ftxui::vbox(get_children(node));
        case MD_BLOCK_P:
            return ftxui::hbox(ftxui::separatorEmpty(), ftxui::vflow(get_children(node)));
        case MD_BLOCK_TABLE:
        case MD_BLOCK_THEAD:
        case MD_BLOCK_TBODY:
        case MD_BLOCK_TR:
        case MD_BLOCK_TH:
            return ftxui::flexbox(get_children(node));
        case MD_BLOCK_TD:
            return ftxui::flexbox(get_children(node)) | ftxui::border;
        }
        return ftxui::emptyElement();
    }

    ftxui::Element add_span(const std::shared_ptr<Node>& node)
    {
        switch(node->span_type_) {
        case MD_SPAN_EM:
        case MD_SPAN_STRONG:
        case MD_SPAN_A:
        case MD_SPAN_IMG:
        case MD_SPAN_CODE:
        case MD_SPAN_DEL:
        case MD_SPAN_LATEXMATH:
        case MD_SPAN_LATEXMATH_DISPLAY:
        case MD_SPAN_WIKILINK:
        case MD_SPAN_U: {
            return ftxui::text(node->text_);
        } break;
        }
        return ftxui::emptyElement();
    }
} // namespace

TView::TView()
{
}

TView::~TView()
{
}

void TView::clear()
{
}

void TView::add(const std::string& text)
{
    MD_PARSER parser = {};
    parser.flags = MD_DIALECT_GITHUB;
    parser.enter_block = enter_block;
    parser.leave_block = leave_block;

    parser.enter_span = enter_span;
    parser.leave_span = leave_span;

    parser.text = parse_text;

    Context context;
    int32_t result = md_parse(text.c_str(), text.length(), &parser, &context);
    if(result != 0) {
        return;
    }
    elements_.push_back(add_node(context.nodes_[0]));
}

ftxui::Element TView::get_top()
{
    return ftxui::vbox(elements_);
}

RawView::RawView()
{
}

    RawView::~RawView()
{
}

    void RawView::clear()
{
        elements_.clear();
}

    void RawView::add(const std::string& text)
{
        elements_.push_back(ftxui::hflow(text));
}

    ftxui::Element RawView::get_top()
{
        return ftxui::vbox(elements_);
}
} // namespace tview