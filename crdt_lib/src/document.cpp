//
// Created by sumeet on 11/23/20.
//

#include "document.h"

#include <algorithm>

namespace nupad {

    Change::Change(PeerId peerId, crdt::ElementId timestamp, const int changeCount,
                   const std::deque<std::shared_ptr<crdt::Operation>> &operations) :
            peerId_(std::move(peerId)),
            timestamp_(std::move(timestamp)),
            changeCount_(changeCount),
            operations_(operations.begin(), operations.end()) {}

    Change::Change(Change &&other) :
            peerId_(std::move(other.peerId_)),
            timestamp_(other.timestamp_),
            changeCount_(other.changeCount_),
            operations_(std::move(other.operations_)) {
    }

    const PeerId &Change::getPeerId() const {
        return peerId_;
    }

    const crdt::ElementId &Change::getTimestamp() const {
        return timestamp_;
    }

    int Change::getChangeCount() const {
        return changeCount_;
    }

    std::deque<std::shared_ptr<crdt::Operation>> &Change::getOperations() {
        return operations_;
    }

    Change &Change::operator=(Change &&other) noexcept {
        if (this != &other) {
            peerId_ = std::move(other.peerId_);
            timestamp_ = std::move(other.timestamp_);
            operations_ = std::move(other.operations_);
        }
        return *this;
    }

    Document::Document(PeerId peerId, std::string name) :
            peerId_(std::move(peerId)), name_(std::move(name)),
            clock_(peerId_), localClockUpdate_(peerId_), nextTimestampByPeerId_(peerId),
            list_([&]() { return this->getNextElementId(); }) {
        changeCounters_.insert(std::make_pair(peerId_, clock::VectorClock(peerId_)));
    }

    void Document::insert(int index, char value) {
        auto insertOp = list_.insert(index, value);
        addClockUpdateToChanges();
        operations_.push_back(std::make_shared<crdt::Operation>(insertOp));
    }

    void Document::remove(int index) {
        auto deleteOp = list_.remove(index);
        addClockUpdateToChanges();
        operations_.push_back(std::make_shared<crdt::Operation>(deleteOp));
    }

    bool Document::hasChange() {
        return !operations_.empty();
    }

    Change Document::getChange() {
        addClockUpdateToChanges();
        auto changeCount = changeCounters_.at(peerId_).tick();
        auto message = Change(peerId_, getNextElementId(), changeCount, operations_);
        operations_.clear();
        return message;
    }

    void Document::processChange(Change &change) {
        change.getOperations().push_back(
                std::make_shared<crdt::Operation>(crdt::ChangeProcessedOperation(change.getChangeCount())));

        if (receiveBuffer_.find(change.getPeerId()) == receiveBuffer_.end()) {
            receiveBuffer_[change.getPeerId()] = std::vector<Change>();
        }
        auto &buffer = receiveBuffer_.at(change.getPeerId());
        // Sorting changes to maintain the total order of the changes
        std::sort(buffer.begin(), buffer.end(), [](const Change &lhs, const Change &rhs) {
            // TODO: sort based on changeCounter
            return lhs.getTimestamp() < rhs.getTimestamp();
        });

        applyCasuallyReadyChanges();
    }

    void Document::addClockUpdateToChanges() {
        if (!localClockUpdate_.getState().empty()) {
            crdt::ClockUpdateOperation clockUpdateOperation(nextTimestampByPeerId_.getMyTick(),
                                                            localClockUpdate_.getState());
            operations_.push_back(std::make_shared<crdt::Operation>(clockUpdateOperation));
            localClockUpdate_.reset();
            localClockUpdate_.tick(nextTimestampByPeerId_.getMyTick());
        }
    }

    crdt::ElementId Document::getNextElementId() {
        return crdt::ElementId(peerId_, this->clock_.tick());
    }

    void Document::applyCasuallyReadyChanges() {
        std::optional<PeerId> causallyReadyPeer = std::nullopt;
        do {
            causallyReadyPeer = getCasuallyReadyPeer();

        } while (causallyReadyPeer.has_value());
    }

    bool Document::isCausallyReady(const PeerId &peerId) {
        // TODO: implement this
        return true;
    }

    std::optional<PeerId> Document::getCasuallyReadyPeer() {
        for (const auto &pair: receiveBuffer_) {
            if (isCausallyReady(pair.first) && !pair.second.empty()) {
                return std::make_optional<PeerId>(pair.first);
            }
        }
        return std::nullopt;
    }

    void Document::applyRemotePeerChanges(const PeerId &remotePeerId) {
        while (true) {
//            auto changes = receiveBuffer_.at(remotePeerId);
//            for (auto &change : changes) {
//                auto operations = change.getOperations();
//                while (!operations.empty()) {
//                    auto operation = operations.front().get();
//                    operations.pop_front();
//                    applyOperation(*operation);
//                }
//            }
        }
    }

    void Document::applyOperation(const crdt::Operation &operation) {
        switch (operation.getOperationType()) {
            case crdt::Insert:
            case crdt::Delete:
                if (clock_.getMyTick() < operation.getTimeStamp()->getTick()) {
                    clock_.tick(operation.getTimeStamp()->getTick());
                }
                list_.apply(operation);
                break;
            case crdt::ClockUpdate:
                applyClockUpdateOperation(dynamic_cast<const crdt::ClockUpdateOperation *>(&operation));
                break;
            case crdt::ChangeProcessed:
                applyChangeProcessedOperation(dynamic_cast<const crdt::ChangeProcessedOperation *>(&operation));
                break;
            default:
                LOG(FATAL) << "Unknown operation: " << operation;
        }
    }

    void Document::applyClockUpdateOperation(const crdt::ClockUpdateOperation *clockUpdateOperation) {
        CHECK_NOTNULL(clockUpdateOperation);

    }

    void Document::applyChangeProcessedOperation(const crdt::ChangeProcessedOperation *changeProcessedOperation) {
        CHECK_NOTNULL(changeProcessedOperation);

    }
}