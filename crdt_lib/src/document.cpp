//
// Created by sumeet on 11/23/20.
//

#include "document.h"

#include <algorithm>
#include <utility>

namespace nupad {

    Document::Document(PeerId peerId, std::string name) :
            peerId_(std::move(peerId)), name_(std::move(name)), logicalTS_(0),
            changeClock_(peerId_), list_([&]() { return this->getNextElementId(); }) {
        bufferedChange_ = new common::Change;
        LOG(INFO) << "Document: " << name_ << " initialized";
    }

    void Document::insert(const int index, const char value) {
        insert(index, std::string{value});
    }

    void Document::insert(const int index, const std::string &value) {
        auto *opPtr = bufferedChange_->add_operations();
        list_.insert(index, value, opPtr);
    }

    void Document::remove(const int index) {
        auto *opPtr = bufferedChange_->add_operations();
        list_.remove(index, opPtr);
    }

    bool Document::hasChange() {
        return bufferedChange_->operations_size() != 0;
    }

    common::Change Document::getChange() {
        CHECK(hasChange()) << "getChange called when no document has no changes";
        bufferedChange_->mutable_change_id()->set_peer_id(peerId_);
        bufferedChange_->mutable_change_id()->mutable_tick()->set_value(changeClock_.getMyTick() + 1);

        for (const auto &pair: changeClock_.getState()) {
            common::Tick tick;
            tick.set_value(pair.second);
            (*bufferedChange_->mutable_dependencies())[pair.first] = tick;
        }

        common::Change change(*bufferedChange_);
        changeClock_.tick();

        delete bufferedChange_;
        bufferedChange_ = new common::Change;
        return change;
    }

    void Document::processChange(common::Change &change) {
        // TODO: test applying same chaanges
        CHECK(change.has_change_id()) << "change id not present";
        CHECK_GT(change.operations_size(), 0) << "change should contain at least one operation";
        auto const &changeId = change.change_id();
        if (isDuplicateChange(changeId)) {
            LOG(WARNING) << "Duplicate Change, changeId: " << changeId
                         << ", operation Count: " << change.operations_size();
            return;
        }

        if (receiveBuffer_.find(changeId.peer_id()) == receiveBuffer_.end()) {
            receiveBuffer_[changeId.peer_id()] = std::deque<common::Change>();
        }
        auto &buffer = receiveBuffer_.at(changeId.peer_id());
        buffer.push_back(change);
        // Sorting changes to maintain the total order of the changes
        // TODO: test if being sorted
        std::sort(buffer.begin(), buffer.end(), [](const common::Change &lhs, const common::Change &rhs) {
            return lhs.change_id() < rhs.change_id();
        });

        applyAllCasuallyReadyChange();
    }

    common::ID Document::getNextElementId() {
        common::ID nextElementId;
        nextElementId.set_peer_id(peerId_);
        nextElementId.mutable_tick()->set_value(++logicalTS_);
        return nextElementId;
    }

    void Document::applyAllCasuallyReadyChange() {
        for (auto &pair: receiveBuffer_) {
            const auto &peerId = pair.first;
            auto &changeDeque = pair.second;
            while (!changeDeque.empty() && isCausallyReady(changeDeque.front())) {
                auto &change = changeDeque.front();
                const auto &changeId = change.change_id();
                CHECK_EQ(peerId, changeId.peer_id());
                applyChange(change);
                appliedChangeIDs.insert(changeId);
                changeClock_.update(changeId.peer_id(), changeId.tick().value());
                changeDeque.pop_front();
            }
        }
    }

    bool Document::isCausallyReady(const common::Change &change) {
        const auto &dependencies = change.dependencies();
        for (const auto &pair: dependencies) {
            const auto &peerId = pair.first;
            const auto &minTickRequired = pair.second;
            if (changeClock_.getTick(peerId) < minTickRequired.value()) {
                return false;
            }
        }

        return true;
    }

    bool Document::isDuplicateChange(const common::ID &changeId) {
        return appliedChangeIDs.find(changeId) != appliedChangeIDs.end();
    }

    void Document::applyChange(common::Change &change) {
        for (const auto &operation: change.operations()) {
            logicalTS_ = std::max(logicalTS_, operation.timestamp().tick().value());
            list_.apply(operation);
        }
    }

    std::string Document::getString() {
        std::string contents;
        contents.reserve(list_.size());
        for (auto const &ch: list_.getContents()) {
            contents += ch;
        }
        return contents;
    }

    size_t Document::size() {
        return list_.size();
    }
}