#include "tview.h"
#include <cassert>
#include <string>
#include <memory>
#include "md4c.h"

namespace tview
{
namespace
{
    static int32_t MD_SPAN_EMPTY = MD_SPAN_U+1;

    struct Node
    {
        int32_t block_type_;
        int32_t detail_;
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
                assert(context->nodes_.size()<=0);
                auto doc = std::shared_ptr<Node>(new Node{});
                doc->block_type_ = MD_BLOCK_DOC;
                doc->span_type_ = -1;
                context->nodes_.emplace_back(doc);
            } break;

            case MD_BLOCK_QUOTE:
            case MD_BLOCK_UL:
            case MD_BLOCK_OL:
            case MD_BLOCK_LI: {
                assert(context->nodes_.back()->span_type_<0);
                auto p = std::shared_ptr<Node>(new Node{});
                p->block_type_ = type;
                p->span_type_ = -1;
                context->nodes_.back()->children_.push_back(p);
                context->nodes_.emplace_back(p);
            } break;

            case MD_BLOCK_HR: {
                assert(context->nodes_.back()->span_type_<0);
                auto hr = std::shared_ptr<Node>(new Node{});
                hr->block_type_ = MD_BLOCK_HR;
                hr->span_type_ = -1;
                context->nodes_.back()->children_.push_back(hr);
            } break;

            case MD_BLOCK_H: {
                assert(context->nodes_.back()->span_type_<0);
                auto h = std::shared_ptr<Node>(new Node{});
                h->block_type_ = MD_BLOCK_H;
                h->span_type_ = -1;
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
                assert(context->nodes_.back()->span_type_<0);
                auto p = std::shared_ptr<Node>(new Node{});
                p->block_type_ = type;
                p->span_type_ = -1;
                context->nodes_.back()->children_.push_back(p);
                context->nodes_.emplace_back(p);
            } break;
            }
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
            case MD_SPAN_U:{
                auto p = std::shared_ptr<Node>(new Node{});
                p->block_type_ = -1;
                p->span_type_ = type;
                context->nodes_.back()->children_.push_back(p);
                context->nodes_.emplace_back(p);
            } break;
            }
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
            case MD_SPAN_U:{
                context->nodes_.pop_back();
            } break;
            }
    }

        int parse_text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata)
    {
            Context* context = (Context*)userdata;
            switch(type) {
            case MD_TEXT_NORMAL:{
                context->nodes_.back()->text_.append(text, size);
            }
                               break;
            case MD_TEXT_NULLCHAR:{
            }
                               break;
            case MD_TEXT_BR:
            case MD_TEXT_SOFTBR:
            case MD_TEXT_ENTITY:
            case MD_TEXT_CODE:
            case MD_TEXT_HTML:
            case MD_TEXT_LATEXMATH:
            case MD_SPAN_WIKILINK:
            case MD_SPAN_U:{
                context->nodes_.back()->text_.append(text, size);
            } break;
            }
    }
}

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
        if(result != 0){
            return;
        }
}

}